/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "metrics_horizvert.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
namespace metrics {
#if 0
}
#endif

horizvert_axi::horizvert_axi()
	: horiz(0, 0, 0),
	  vert(0, 0, 0)
{
}

horizvert_axi::horizvert_axi(const axis &horiz,
			     const axis &vert)
	: horiz(horiz),
	  vert(vert)
{
}

horizvertObj::horizvertObj()=default;

horizvertObj::horizvertObj(const horizvert_axi &c): horizvert_axi(c),
						    original_axis(c)
{
}

horizvertObj::~horizvertObj()=default;

#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
