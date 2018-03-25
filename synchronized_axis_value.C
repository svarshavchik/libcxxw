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

void my_synchronized_axis::synchronize(ONLY IN_THREAD,
				       synchronized_values::lock &lock)
{
	lock->recalculate(IN_THREAD, value_list_iterator);
}

///////////////////////////////////////////////////////////////////////

void synchronized_axis_values_t
::recalculate(ONLY IN_THREAD,
	      std::list<synchronized_axis_value>::iterator iter)
{
	LOG_FUNC_SCOPE(recalculate_loggerObj);

	// Dynamically derive how many axises get synchronized.

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
			(*b)->updated(IN_THREAD, derived_values);
		} CATCH_EXCEPTIONS;
	}
}

LIBCXXW_NAMESPACE_END
