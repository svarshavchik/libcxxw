/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "background_color.H"
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

background_colorObj::background_colorObj()=default;

background_colorObj::~background_colorObj()=default;


class LIBCXX_HIDDEN nonThemeBackgroundColorObj : public background_colorObj {

 public:

	const const_picture fixed_color;

	nonThemeBackgroundColorObj(const const_picture &fixed_color)
		: fixed_color(fixed_color)
	{
	}

	~nonThemeBackgroundColorObj()=default;

	const_picture get_current_color() override
	{
		return fixed_color;
	}
};

background_color
background_colorBase::create_background_color(const const_picture &color)
{
	return ref<nonThemeBackgroundColorObj>::create(color);
}

LIBCXXW_NAMESPACE_END
