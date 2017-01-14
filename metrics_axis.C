/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_axis.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

exception axis::invalid_infinite()
{
	return EXCEPTION("Cannot specify an infinite minimum or preferred metric.");

}

exception axis::invalid_minimum()
{
	return EXCEPTION("Preferred metric less than the minimum one,");
}

exception axis::invalid_maximum()
{
	return EXCEPTION("Preferred metric is more than the maximum one.");
}


axis axis::increase_minimum_by(dim_t howmuch) const
{
	auto m=max_increase_minimum_by();

	if (m < howmuch)
		howmuch=m;

	dim_t new_minimum=(dim_squared_t::value_type)(howmuch + minimum());

	auto new_preferred=preferred();

	if (new_preferred < new_minimum)
		new_preferred=new_minimum;

	auto new_maximum=maximum();

	if (new_maximum < new_preferred)
		new_maximum=new_preferred;

	return {new_minimum, new_preferred, new_maximum};
}

axis axis::decrease_maximum_by(dim_t howmuch) const
{
	auto m=max_decrease_maximum_by();

	if (m < howmuch)
		howmuch=m;

	dim_t new_maximum=maximum()-howmuch;

	auto new_preferred=preferred();

	if (new_preferred > new_maximum)
		new_preferred=new_maximum;

	return {minimum(), new_preferred, new_maximum};
}

derived_axis_obj create_derived_axis_obj()
{
	return x::derivedvalues<axis>
		::create( []
			  {
				  return derivedaxis();
			  },
			  []
			  (auto &derived, auto &axis)
			  {
				  derived(axis);
			  },
			  []
			  (const auto &derived)
			  {
				  return axis(derived);
			  });
}

std::ostream &operator<<(std::ostream &o,
			 const axis &a)
{
	return o << "minimum=" << a.minimum()
		 << ", preferred=" << a.preferred()
		 << ", maximum=" << a.maximum();
}
#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
