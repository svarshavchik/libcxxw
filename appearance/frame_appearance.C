/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/frame_appearance.H"

LIBCXXW_NAMESPACE_START

frame_appearance_properties::frame_appearance_properties()
	: border{"frame_border"},
	  title_indent{"frame_title_indent"},
	  frame_background{"frame_background"},
	  hpad{"frame_hpad"},
	  vpad{"frame_vpad"}
{
}

frame_appearance_properties::~frame_appearance_properties()=default;

frame_appearanceObj::frame_appearanceObj()=default;

frame_appearanceObj::~frame_appearanceObj()=default;

frame_appearanceObj::frame_appearanceObj
(const frame_appearanceObj &o)
	: frame_appearance_properties{o}
{
}

const_frame_appearance frame_appearanceObj
::do_modify(const function<void (const frame_appearance &)> &closure) const
{
	auto copy=frame_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_frame_appearance &frame_appearance_base::theme()
{
	static const const_frame_appearance config=
		const_frame_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
