/*
** Copyright 2017 Double Precision, Inc.
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
		radius == o.radius && dashes == o.dashes &&
		colors == o.colors;
}

LIBCXXW_NAMESPACE_END
