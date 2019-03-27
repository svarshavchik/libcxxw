/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/progressbar_appearance.H"

LIBCXXW_NAMESPACE_START

progressbar_appearance_properties::progressbar_appearance_properties()
	: border{"progressbar"},
	  minimum_width{"progressbar"},
	  background_color{"progressbar_background_color"},
	  foreground_color{"progressbar_foreground_color"},
	  label_font{theme_font{"progressbar"}},
	  slider_color{"progressbar"}
{
}

progressbar_appearance_properties::~progressbar_appearance_properties()=default;

progressbar_appearanceObj::progressbar_appearanceObj()=default;

progressbar_appearanceObj::~progressbar_appearanceObj()=default;

progressbar_appearanceObj::progressbar_appearanceObj
(const progressbar_appearanceObj &o)
	: progressbar_appearance_properties{o}
{
}

const_progressbar_appearance progressbar_appearanceObj
::do_modify(const function<void (const progressbar_appearance &)> &closure) const
{
	auto copy=progressbar_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_progressbar_appearance &progressbar_appearance_base::theme()
{
	static const const_progressbar_appearance config=
		const_progressbar_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
