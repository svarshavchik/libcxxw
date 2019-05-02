/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scrollbar_images_appearance.H"

LIBCXXW_NAMESPACE_START

static scrollbar_images_appearance create_images(const char *scroll_low,
						 const char *scroll_high,
						 const char *start,
						 const char *handle,
						 const char *end,
						 const char *suffix)
{
	auto images=scrollbar_images_appearance::create();

	std::string scroll{"scroll-"};
	std::string scrollbar{"scrollbar-"};
	std::string knob{"scrollbar-knob-"};

	images->scroll_low=scroll + scroll_low + suffix;
	images->scroll_high=scroll + scroll_high + suffix;
	images->knob_start=knob + start + suffix;
	images->knob_handle=scrollbar + handle + suffix;
	images->knob_end=knob + end + suffix;

	return images;
}

scrollbar_images_appearance_properties::scrollbar_images_appearance_properties()
{
}

scrollbar_images_appearance_properties::~scrollbar_images_appearance_properties()=default;

scrollbar_images_appearanceObj::scrollbar_images_appearanceObj()=default;

scrollbar_images_appearanceObj::~scrollbar_images_appearanceObj()=default;

scrollbar_images_appearanceObj::scrollbar_images_appearanceObj
(const scrollbar_images_appearanceObj &o)
	: scrollbar_images_appearance_properties{o}
{
}

const_scrollbar_images_appearance scrollbar_images_appearanceObj
::do_modify(const function<void (const scrollbar_images_appearance &)> &closure) const
{
	auto copy=scrollbar_images_appearance::create(*this);
	closure(copy);
        return copy;
}



const const_scrollbar_images_appearance &
scrollbar_images_appearance_base::horizontal1()
{
	static const_scrollbar_images_appearance config=
		create_images("left", "right",
				 "left", "horiz", "right", "1");

	return config;
}

const const_scrollbar_images_appearance &scrollbar_images_appearance_base
::horizontal2()
{
	static const_scrollbar_images_appearance config=
		create_images("left", "right",
				 "left", "horiz", "right", "2");

	return config;
}

const const_scrollbar_images_appearance &scrollbar_images_appearance_base
::vertical1()
{
	static const_scrollbar_images_appearance config=
		create_images("up", "down",
			      "top", "vert", "bottom", "1");

	return config;
}

const const_scrollbar_images_appearance &scrollbar_images_appearance_base
::vertical2()
{
	static const_scrollbar_images_appearance config=
		create_images("up", "down",
			      "top", "vert", "bottom", "2");

	return config;
}

LIBCXXW_NAMESPACE_END
