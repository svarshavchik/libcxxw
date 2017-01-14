/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_grid_axisrange.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

grid_axisrange::grid_axisrange(const grid_xy &start,
			       const grid_xy &end)
	: start(start), end(end)
{
	if (end < start)
		throw EXCEPTION("ending grid position " << end
				<< " prior to starting grid position "
				<< start);
}

#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
