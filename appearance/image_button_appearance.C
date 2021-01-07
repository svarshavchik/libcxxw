/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/image_button_appearance.H"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

image_button_appearance_properties
::image_button_appearance_properties(const std::vector<std::string> &images)
	: alignment{valign::middle},
	  focus_border{focus_border_appearance::base::thin_theme()},
	  images{images}
{
}

image_button_appearance_properties::~image_button_appearance_properties()=default;

image_button_appearanceObj
::image_button_appearanceObj(const std::vector<std::string> &images)
	: image_button_appearance_properties{images}
{
}

image_button_appearanceObj::~image_button_appearanceObj()=default;

image_button_appearanceObj::image_button_appearanceObj
(const image_button_appearanceObj &o)
	: image_button_appearance_properties{o}
{
}

const_image_button_appearance image_button_appearanceObj
::do_modify(const function<void (const image_button_appearance &)> &closure) const
{
	auto copy=image_button_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct image_button_appearance_base_checkbox_themeObj : virtual public obj {

	const const_image_button_appearance config=const_image_button_appearance::create
		(std::vector<std::string>{"checkbox1", "checkbox2", "checkbox3"
				});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance_base::checkbox_theme()
{
	return singleton<image_button_appearance_base_checkbox_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct image_button_appearance_base_radio_themeObj : virtual public obj {

	const const_image_button_appearance config=const_image_button_appearance::create
		(std::vector<std::string>{"radio1", "radio2"});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance_base::radio_theme()
{
	return singleton<image_button_appearance_base_radio_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct image_button_appearance_base_item_themeObj : virtual public obj {

	const const_image_button_appearance config=const_image_button_appearance::create
		(std::vector<std::string>{"itemdelete1", "itemdelete2"});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance_base::item_theme()
{
	return singleton<image_button_appearance_base_item_themeObj>::get()->config;
}

// Scroll buttons, just above the main page area. Align the scroll
// buttons to the bottom, where the page area is. Use a visible
// focusoff border, to avoid ugly empty separation between the page
// border and the visible button, when it does not have input focus.

static auto create_book_button(const std::vector<std::string> &images)
{
	auto appearance=image_button_appearance::create(images);

	appearance->alignment=valign::bottom;
	appearance->focus_border=
		focus_border_appearance::base::visible_thin_theme();

	return appearance;
}

namespace {
#if 0
}
#endif

struct image_button_appearance_base_book_left_themeObj : virtual public obj {

	const const_image_button_appearance config=create_book_button({"scroll-left1", "scroll-left2"});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance_base::book_left_theme()
{
	return singleton<image_button_appearance_base_book_left_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct image_button_appearance_base_book_right_themeObj : virtual public obj {

	const const_image_button_appearance config=create_book_button({"scroll-right1", "scroll-right2"});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance_base::book_right_theme()
{
	return singleton<image_button_appearance_base_book_right_themeObj>::get()->config;
}

static auto create_date_popup_button(const std::vector<std::string> &images)
{
	auto appearance=image_button_appearance::create(images);

	appearance->alignment=valign::bottom;

	return appearance;
}

namespace {
#if 0
}
#endif

struct base_date_popup_left_themeObj : virtual public obj {

	const const_image_button_appearance config=create_date_popup_button({"scroll-left1", "scroll-left2"});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance::base::date_popup_left_theme()
{
	return singleton<base_date_popup_left_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct image_button_appearance_base_date_popup_right_themeObj : virtual public obj {

	const const_image_button_appearance config=create_date_popup_button({"scroll-right1", "scroll-right2"});

};

#if 0
{
#endif
}

const_image_button_appearance 
image_button_appearance_base::date_popup_right_theme()
{
	return singleton<image_button_appearance_base_date_popup_right_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
