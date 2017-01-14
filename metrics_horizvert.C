/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/metrics/horizvert.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

horizvertObj::horizvertObj(dim_t h_minimum,
			   dim_t h_preferred,
			   dim_t h_maximum,
			   dim_t v_minimum,
			   dim_t v_preferred,
			   dim_t v_maximum)
	: horiz(h_minimum, h_preferred, h_maximum),
	  vert(v_minimum, v_preferred, v_maximum)
{
}

horizvertObj::~horizvertObj()=default;

#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
