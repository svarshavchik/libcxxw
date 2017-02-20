/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/picture.H"
#include "border_info.H"

LIBCXXW_NAMESPACE_START

border_info::border_info()=default;

border_info::~border_info()=default;

bool border_info::no_border() const
{
	return width == 0 || height == 0 || colors.empty();
}

bool border_info::operator==(const border_info &o) const
{
	return width == o.width && height == o.height &&
		hradius == o.hradius && vradius == o.vradius &&
		dashes == o.dashes && colors == o.colors;
}

bool border_info::compare(const border_info &o) const
{
	if (width + height < o.width + o.height)
		return true;
	if (o.width + o.height < width + height)
		return false;

	if (hradius+vradius < o.hradius + o.vradius)
		return true;
	if (o.hradius+o.vradius < hradius + vradius)
		return false;

	if (dashes < o.dashes)
		return true;

	if (o.dashes < o.dashes)
		return false;

	return colors < o.colors;
}

LIBCXXW_NAMESPACE_END
