/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dim_axis_arg.H"
#include <math.h>

LIBCXXW_NAMESPACE_START

dim_axis_arg::dim_axis_arg() : minimum{0}, preferred{0}, maximum{NAN}
{
}

dim_axis_arg::dim_axis_arg(const dim_arg &a)
	: minimum{a}, preferred{a}, maximum{a}
{
}

dim_axis_arg::dim_axis_arg(const dim_arg &minimum,
			   const dim_arg &preferred)
	: minimum{minimum}, preferred{preferred}, maximum{NAN}
{
}

dim_axis_arg::dim_axis_arg(const dim_arg &minimum,
			   const dim_arg &preferred,
			   const dim_arg &maximum)
	: minimum{minimum}, preferred{preferred},
	  maximum{maximum}
{
}

dim_axis_arg::~dim_axis_arg()=default;

LIBCXXW_NAMESPACE_END
