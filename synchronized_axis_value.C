/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "synchronized_axis_value.H"
#include "synchronized_axis_impl.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::synchronized_axis::recalculate,
		    recalculate_loggerObj);

synchronized_axis_valueObj::synchronized_axis_valueObj()=default;

synchronized_axis_valueObj::~synchronized_axis_valueObj()=default;

//==========================================================================

// The constructor inserts the new value into the derived axis list, and
// returns the iterator.

static inline auto insert_into_list(const synchronized_axis &axis,
				    const synchronized_axis_value &my_value)
{
	synchronized_values::lock lock{axis->impl->values};

	lock->all_values.push_back(my_value);
	return lock->all_values.end();
}

my_synchronized_axis
::my_synchronized_axis(const synchronized_axis &axis,
		       const synchronized_axis_value &my_value)
	: axis{axis},
	  my_value{my_value},
	  value_list_iterator{insert_into_list(axis, my_value)}
{
}

my_synchronized_axis::~my_synchronized_axis()
{
	synchronized_values::lock lock{axis->impl->values};

	if (value_list_iterator != lock->all_values.end())
		lock->all_values.erase(value_list_iterator);
}

void my_synchronized_axis::removed_from_container(ONLY IN_THREAD)
{
	synchronized_values::lock lock{axis->impl->values};

	auto e=lock->all_values.end();

	if (value_list_iterator == e)
		return;

	lock->all_values.erase(value_list_iterator);
	value_list_iterator=e;

	lock->recalculate(IN_THREAD, e);
}

my_synchronized_axis::lock::lock(my_synchronized_axis &me)
	: synchronized_values::lock{me.axis->impl->values},
	  me{me}
{
}

my_synchronized_axis::lock::~lock()=default;

bool my_synchronized_axis::lock::has_synchronized_values(ONLY IN_THREAD)
{
	// If this is the only element in the list, and we are not
	// asking for any minimum width, we don't need to do anything here.

	return (*this)->all_values.size() > 1
		|| me.my_value->minimum(IN_THREAD) > 0;
}

void my_synchronized_axis::lock
::update_values(ONLY IN_THREAD,	const std::vector<metrics::axis> &values)
{
	update_values(IN_THREAD, values, 0, {});
}

void my_synchronized_axis::lock
::update_values(ONLY IN_THREAD,
		const std::vector<metrics::axis> &values,
		dim_t minimum,
		const std::unordered_map<size_t, int> &requested_col_widths)
{
	// Compare the passed-in values, and something_changed only if they
	// are different.
	//
	// We diligently compare each value individually, and burn electrons
	// on copying the entire container only if absolutely needed. This is
	// a hot path.

	bool something_changed=false;

	if (me.my_value->values(IN_THREAD) != values)
	{
		something_changed=true;
		me.my_value->values(IN_THREAD)=values;
	}

	if (me.my_value->minimum(IN_THREAD) != minimum)
	{
		something_changed=true;
		me.my_value->minimum(IN_THREAD)=minimum;
	}

	if (me.my_value->requested_col_widths(IN_THREAD) !=
	    requested_col_widths)
	{
		something_changed=true;
		me.my_value->requested_col_widths(IN_THREAD)=
			requested_col_widths;
	}

	if (something_changed)
		(*this)->recalculate(IN_THREAD, me.value_list_iterator);
}

///////////////////////////////////////////////////////////////////////

synchronized_axis_values_t
::synchronized_axis_values_t(synchronized_axisObj::implObj &me)
	: me{me}
{
}

void synchronized_axis_values_t
::recalculate(ONLY IN_THREAD,
	      std::list<synchronized_axis_value>::iterator iter)
{
	LOG_FUNC_SCOPE(recalculate_loggerObj);

	auto new_derived_values=me.compute_derived_values(IN_THREAD,
							  *this);

	// If the computed synchronized axises are unchanged, we're done.
	if (derived_values == new_derived_values)
		return;

	// Save new synchronized axises
	derived_values=std::move(new_derived_values);

	// Now, invoke all individual axises that got synchronized what the
	// new synchronized values are.

	for (auto b=all_values.begin(), e=all_values.end(); b != e; ++b)
	{
		if (b == iter) continue;	// The one to skip

		try {
			(*b)->synchronized_axis_updated(IN_THREAD,
							derived_values);
		} CATCH_EXCEPTIONS;
	}
}

std::vector<metrics::derivedaxis> synchronized_axisObj::implObj
::compute_derived_values(ONLY IN_THREAD,
			 const synchronized_axis_values_t &values)
{
	std::vector<metrics::derivedaxis> new_derived_values;

	for (const auto &v:values.all_values)
	{
		size_t n=v->values(IN_THREAD).size();

		if (n > new_derived_values.size())
			new_derived_values.resize(n);

		auto iter=new_derived_values.begin();

		for (const auto &a:v->values(IN_THREAD))
		{
			(*iter)(a);
			++iter;
		}
	}

	// Compute the total minimum value.

	dim_t total_minimum=0;

	for (const auto &derived_value:new_derived_values)
		total_minimum=dim_t::truncate
			(total_minimum+derived_value.minimum());

	// Now, find the a synchronized value with the largest minimum.

	auto largest_minimum=values.all_values.end();

	for (auto b=values.all_values.begin(),
		     e=values.all_values.end(); b != e; ++b)
	{
		auto &v=*b;

		// If this synchronized value's minimum is less than
		// the total_minimum, skip it.

		if (v->minimum(IN_THREAD) <= total_minimum)
			continue;

		if (largest_minimum != e &&
		    (*largest_minimum)->minimum(IN_THREAD)
		    >= v->minimum(IN_THREAD))
			continue;

		largest_minimum=b;
	}

	if (largest_minimum != values.all_values.end())
	{
		auto &v=*largest_minimum;

		dim_squared_t::value_type denominator=0;

		// Sum up the total weights of all adjustable columns,
		// if any were specified.

		denominator=0;

		for (size_t i=new_derived_values.size(); i; )
		{
			--i;
			auto iter=v->requested_col_widths(IN_THREAD).find(i);

			if (iter != v->requested_col_widths(IN_THREAD).end())
				denominator += iter->second;
		}

		// If none were specified, scale all columns equally.
		auto all_weights_are_one=denominator == 0;

		if (all_weights_are_one)
			denominator=new_derived_values.size();

		// denominator > 0
		//
		// v->minimum(IN_THREAD) > total_minimum
		//
		// Firstly: here's how much to increase the minimums by:

		dim_squared_t::value_type
			total_to_distribute=
			dim_squared_t::truncate(v->minimum(IN_THREAD)
						-total_minimum);

		// What we add to each column:
		//
		// (total_to_distribute / denominator) * requested_col_width.
		//
		// "denominator" is the sum total of all columns'
		// requested_col-width, according to the above, so this
		// will end up adding "total_to_distribute" to all columns,
		// proportionately.
		//
		// We multiply total_to_distribute * requested_col_width,
		// then divide by denominator, and carry forward the remainder.
		//
		// The carry-forward is held in nominator:

		dim_squared_t::value_type nominator=0;

		for (size_t i=new_derived_values.size(); i; )
		{
			--i;

			dim_squared_t::value_type n=0;

			auto iter=v->requested_col_widths(IN_THREAD).find(i);

			if (iter != v->requested_col_widths(IN_THREAD).end())
				n=iter->second;

			if (all_weights_are_one)
				n=1;

			auto product=nominator + n * total_to_distribute;

			auto extra=product / denominator;

			nominator=product % denominator;

			metrics::axis &v=new_derived_values[i];

			v=v.increase_minimum_by(extra);
		}
	}
	return new_derived_values;
}

LIBCXXW_NAMESPACE_END
