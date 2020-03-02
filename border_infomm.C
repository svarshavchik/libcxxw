/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/picture.H"
#include "x/w/border_infomm.H"

LIBCXXW_NAMESPACE_START

border_infomm::border_infomm()=default;

border_infomm::~border_infomm()=default;

bool border_infomm::operator==(const border_infomm &o) const
{
	return width == o.width && height == o.height &&
		rounded == o.rounded &&
		hradius == o.hradius &&
		vradius == o.vradius &&
		width_scale == o.width_scale &&
		height_scale == o.height_scale &&
		hradius_scale == o.hradius_scale &&
		vradius_scale == o.vradius_scale &&
		dashes == o.dashes &&
		color1 == o.color1 && color2 == o.color2;
}

LIBCXXW_NAMESPACE_END
