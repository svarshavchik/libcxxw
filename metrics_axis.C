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
