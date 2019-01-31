/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_grid_pos.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

grid_posObj::grid_posObj()=default;

grid_posObj::~grid_posObj()=default;

void grid_posObj::validate() const
{
	horiz_pos.validate();
	vert_pos.validate();
}
#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
