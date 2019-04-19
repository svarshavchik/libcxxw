/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/dim_axis_arg.H"
#include "x/w/metrics/axis.H"
#include "defaulttheme.H"

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

metrics::axis dim_axis_arg::compute(const const_defaulttheme &theme,
				    themedimaxis wh) const
{
	auto min=theme->get_theme_dim_t(minimum, wh);
	auto pref=theme->get_theme_dim_t(preferred, wh);
	auto max=theme->get_theme_dim_t(maximum, wh);

	if (pref < min)
		pref=min;

	if (max < pref)
		max=pref;

	return {min, pref, max};
}


LIBCXXW_NAMESPACE_END
