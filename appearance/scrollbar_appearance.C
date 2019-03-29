/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scrollbar_appearance.H"

LIBCXXW_NAMESPACE_START

static void default_scrollbar_images(scrollbar_images &images,
				     const char *scroll_low,
				     const char *scroll_high,
				     const char *start,
				     const char *handle,
				     const char *end,
				     const char *suffix)
{
	std::string scroll{"scroll-"};
	std::string scrollbar{"scrollbar-"};
	std::string knob{"scrollbar-knob-"};

	images.scroll_low=scroll + scroll_low + suffix;
	images.scroll_high=scroll + scroll_high + suffix;
	images.knob_start=knob + start + suffix;
	images.knob_handle=scrollbar + handle + suffix;
	images.knob_end=knob + end + suffix;
}

scrollbar_appearance_properties::scrollbar_appearance_properties()
	: background_color{"scrollbar_background_color"},
	  focusoff_border{"scrollbarfocusoff_border"},
	  focuson_border{"scrollbarfocuson_border"}
{
	default_scrollbar_images(horizontal1,
				 "left", "right",
				 "left", "horiz", "right", "1");

	default_scrollbar_images(horizontal2,
				 "left", "right",
				 "left", "horiz", "right", "2");

	default_scrollbar_images(vertical1,
				 "up", "down",
				 "top", "vert", "bottom", "1");

	default_scrollbar_images(vertical2,
				 "up", "down",
				 "top", "vert", "bottom", "2");
}

scrollbar_appearance_properties::~scrollbar_appearance_properties()=default;

scrollbar_appearanceObj::scrollbar_appearanceObj()=default;

scrollbar_appearanceObj::~scrollbar_appearanceObj()=default;

scrollbar_appearanceObj::scrollbar_appearanceObj
(const scrollbar_appearanceObj &o)
	: scrollbar_appearance_properties{o}
{
}

const_scrollbar_appearance scrollbar_appearanceObj
::do_modify(const function<void (const scrollbar_appearance &)> &closure) const
{
	auto copy=scrollbar_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_scrollbar_appearance &scrollbar_appearance_base::theme()
{
	static const const_scrollbar_appearance config=
		const_scrollbar_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
