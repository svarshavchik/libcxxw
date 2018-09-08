/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "synchronized_axis_value.H"
#include "synchronized_axis_impl.H"
#include "catch_exceptions.H"
#include "calculate_borders.H"

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
	return --lock->all_values.end();
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
	return (*this)->all_values.size() > 1;
}

#if 0
void my_synchronized_axis::lock
::update_values(ONLY IN_THREAD,	const std::vector<metrics::axis> &values)
{
	update_values(IN_THREAD, values, 0, {});
}
#endif

void my_synchronized_axis::lock
::update_values(ONLY IN_THREAD,
		const std::vector<metrics::axis> &values,
		const std::unordered_map<size_t, int>
		&requested_col_widths)
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

bool my_synchronized_axis::lock::update_minimum(ONLY IN_THREAD, dim_t minimum)
{
	if (me.my_value->minimum(IN_THREAD) == minimum)
		return false;

	me.my_value->minimum(IN_THREAD)=minimum;
	(*this)->recalculate(IN_THREAD, me.value_list_iterator);
	return true;
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

	bool changed_flag=false;

	{
		std::vector<metrics::derivedaxis> new_derived_values;

		for (const auto &v:all_values)
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

		if (new_derived_values != unscaled_values)
		{
			changed_flag=true;
			std::swap(unscaled_values, new_derived_values);
		}
	}

	{
		auto new_scaled_values=me.scale_derived_values(IN_THREAD,
							       *this,
							       unscaled_values);
		if (new_scaled_values != scaled_values)
		{
			changed_flag=true;
			std::swap(new_scaled_values, scaled_values);
		}
	}

	if (!changed_flag)
		return; // Nothing changed.

	// Now, invoke all individual axises that got synchronized what the
	// new synchronized values are.

	for (auto b=all_values.begin(), e=all_values.end(); b != e; ++b)
	{
		if (b == iter) continue;	// The one to skip

		try {
			(*b)->synchronized_axis_updated(IN_THREAD, *this);
		} CATCH_EXCEPTIONS;
	}
}

std::vector<metrics::axis> synchronized_axisObj::implObj
::scale_derived_values(ONLY IN_THREAD,
		       const synchronized_axis_values_t &values,
		       const std::vector<metrics
		       ::derivedaxis> &derived_values)
{
	std::vector<metrics::axis> new_derived_values{derived_values.begin(),
						      derived_values.end()};

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

		auto &target_width=v->minimum(IN_THREAD);

		// total_minimum is the total of the synchronized axis's
		// minimums, which must be less than *largest_minimum, or v,
		// otherwise we will not be here.
		//
		// total_to_add is by how much we want to embiggen our
		// new_derived_values;

		dim_t total_to_add=target_width-total_minimum;

		dim_t total_can_add=0;
		std::vector<std::tuple<size_t, dim_t>> to_expand;

		dim_squared_t::value_type denominator;

		// First, go through and find all columns that have indicated
		// that they're willing to be embiggened.

		for (size_t i=new_derived_values.size(); i; )
		{
			--i;
			auto iter=v->requested_col_widths(IN_THREAD).find(i);

			if (iter == v->requested_col_widths(IN_THREAD).end())
				continue;

			// iter->second is the percentage of the target_width
			// this column is willing to be expanded to.

			dim_t col_desired_size=dim_t::truncate
				(target_width*iter->second/100);

			// And this is it's current size.
			auto current_size=new_derived_values[i].minimum();

			if (col_desired_size < current_size)
				continue;

			dim_t add_more=col_desired_size-current_size;

			to_expand.emplace_back(i, add_more);
			total_can_add=dim_t::truncate(total_can_add+add_more);
		}

		// So now we have a list of columns, and how much each
		// one can be embiggened, with the sum total of embiggenment
		// being total_can_add.

		if (total_can_add < total_to_add)
		{
			// Ok, we won't get there. However, let's take up
			// everyone's offer, first.

			for (const auto &add_all:to_expand)
			{
				auto &[index, how_much]=add_all;

				metrics::axis &v=new_derived_values[index];
				v=v.increase_minimum_by(how_much);
			}

			// So, this is how much there's left to embiggen:
			total_to_add = total_to_add - total_can_add;

			// Now, let's wipe out to_expand, and set each
			// non-border's column's to_expand to 1,
			// # columns.
			//
			// The scaling code below, then, ends up embiggening
			// each non-border column to (1/# columns)*total_to_add

			to_expand.clear();
			to_expand.reserve(new_derived_values.size()/2+1);

			size_t n=0;

			for (size_t i=CALCULATE_BORDERS_COORD(0);
			     i < new_derived_values.size();
			     CALCULATE_BORDERS_INCR_SPAN(i))
			{
				to_expand.emplace_back(i, 1);
				++n;
			}
			denominator=n;
		}
		else
		{
			// The sum total of to_expand's is total_can_add.
			//
			// So that's our denominator.
			//
			// Set the denominator to total_to_add.
			//
			// The scaling code, below, therefore, embiggens
			// everything by total_to_add in total...

			denominator=(dim_t::value_type)total_can_add;
		}

		// At this point:
		//
		// Multiply total_to_add, the nominator value by the weight
		// of each to_expand divided by denominator.
		//
		// total_to_add is the sum total of what to embiggen all
		// columns by, and we scale it by each to_expand columns
		// by its nominator, divided by the denominator.

		if (denominator == 0)
			to_expand.clear();

		dim_squared_t nominator{0};

		for (const auto &column:to_expand)
		{
			auto &[index, how_much]=column;

			auto product=nominator + how_much * total_to_add;

			auto extra=dim_t::truncate(product / denominator);

			nominator=product % denominator;

			metrics::axis &v=new_derived_values[index];

			v=v.increase_minimum_by(extra);
		}
	}
	return new_derived_values;
}

LIBCXXW_NAMESPACE_END
