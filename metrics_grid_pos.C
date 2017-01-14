/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_grid_pos.H"
#include "x/w/metrics/horizvert.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

grid_posObj::grid_posObj(const grid_axisrange &horiz_pos,
			 const grid_axisrange &vert_pos,
			 const horizvert &horizvert_metrics)
	: horiz_pos(horiz_pos),
	  vert_pos(vert_pos),
	  horizvert_metrics(horizvert_metrics)
{
}

grid_posObj::~grid_posObj()=default;

#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
