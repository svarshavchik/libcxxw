/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scrollbar_images_appearance.H"
#include <x/singleton.H>

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



namespace {
#if 0
}
#endif

struct scrollbar_images_appearance_base_horizontal1Obj : virtual public obj {

	const const_scrollbar_images_appearance config=create_images("left", "right",
				 "left", "horiz", "right", "1");

};

#if 0
{
#endif
}

const_scrollbar_images_appearance 
scrollbar_images_appearance_base::horizontal1()
{
	return singleton<scrollbar_images_appearance_base_horizontal1Obj>::get()->config;
}

namespace {
#if 0
}
#endif

struct scrollbar_images_appearance_base_horizontal2Obj : virtual public obj {

	const const_scrollbar_images_appearance config=create_images("left", "right",
				 "left", "horiz", "right", "2");

};

#if 0
{
#endif
}

const_scrollbar_images_appearance scrollbar_images_appearance_base::horizontal2()
{
	return singleton<scrollbar_images_appearance_base_horizontal2Obj>::get()->config;
}

namespace {
#if 0
}
#endif

struct scrollbar_images_appearance_base_vertical1Obj : virtual public obj {

	const const_scrollbar_images_appearance config=create_images("up", "down",
			      "top", "vert", "bottom", "1");

};

#if 0
{
#endif
}

const_scrollbar_images_appearance scrollbar_images_appearance_base::vertical1()
{
	return singleton<scrollbar_images_appearance_base_vertical1Obj>::get()->config;
}

namespace {
#if 0
}
#endif

struct scrollbar_images_appearance_base_vertical2Obj : virtual public obj {

	const const_scrollbar_images_appearance config=create_images("up", "down",
			      "top", "vert", "bottom", "2");

};

#if 0
{
#endif
}

const_scrollbar_images_appearance scrollbar_images_appearance_base::vertical2()
{
	return singleton<scrollbar_images_appearance_base_vertical2Obj>::get()->config;
}

LIBCXXW_NAMESPACE_END
