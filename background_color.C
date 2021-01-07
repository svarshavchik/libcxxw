/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/background_color.H"
#include "x/w/picture.H"
#include "x/w/impl/element.H"
#include "x/w/screen.H"

LIBCXXW_NAMESPACE_START

background_colorObj::background_colorObj(const screen &background_color_screen)
	: background_color_screen{background_color_screen}
{
}

background_colorObj::~background_colorObj()=default;

background_color background_colorObj
::get_background_color_for_element(ONLY IN_THREAD,
				   elementObj::implObj &e)
{
	auto &pos=e.data(IN_THREAD).current_position;

	return get_background_color_for(IN_THREAD, e,
					pos.width, pos.height);
}

LIBCXXW_NAMESPACE_END
