/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/image_button_appearance.H"

LIBCXXW_NAMESPACE_START

image_button_appearance_properties
::image_button_appearance_properties(const std::vector<std::string> &images)
	: alignment{valign::middle},
	  focusoff_border{"thin_inputfocusoff_border"},
	  focuson_border{"thin_inputfocuson_border"},
	  images{images}
{
}

void image_button_appearanceObj::set_distinct_focusoff_border()
{
	focusoff_border="thin_inputfocusoff_border_color2";
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

const const_image_button_appearance &
image_button_appearance_base::checkbox_theme()
{
	static const const_image_button_appearance config=
		const_image_button_appearance::create
		(std::vector<std::string>{"checkbox1", "checkbox2", "checkbox3"
				});

	return config;
}

const const_image_button_appearance &
image_button_appearance_base::radio_theme()
{
	static const const_image_button_appearance config=
		const_image_button_appearance::create
		(std::vector<std::string>{"radio1", "radio2"});

	return config;
}

const const_image_button_appearance &
image_button_appearance_base::item_theme()
{
	static const const_image_button_appearance config=
		const_image_button_appearance::create
		(std::vector<std::string>{"itemdelete1", "itemdelete2"});

	return config;
}

// Scroll buttons, just above the main page area. Align the scroll
// buttons to the bottom, where the page area is. Use a visible
// focusoff border, to avoid ugly empty separation between the page
// border and the visible button, when it does not have input focus.

static auto create_book_button(const std::vector<std::string> &images)
{
	auto appearance=image_button_appearance::create(images);

	appearance->alignment=valign::bottom;
	appearance->set_distinct_focusoff_border();

	return appearance;
}

const const_image_button_appearance &
image_button_appearance_base::book_left_theme()
{
	static const const_image_button_appearance config=
		create_book_button({"scroll-left1", "scroll-left2"});

	return config;
}

const const_image_button_appearance &
image_button_appearance_base::book_right_theme()
{
	static const const_image_button_appearance config=
		create_book_button({"scroll-right1", "scroll-right2"});

	return config;
}

static auto create_date_popup_button(const std::vector<std::string> &images)
{
	auto appearance=image_button_appearance::create(images);

	appearance->alignment=valign::bottom;

	return appearance;
}

const const_image_button_appearance &
image_button_appearance::base::date_popup_left_theme()
{
	static const const_image_button_appearance config=
		create_date_popup_button({"scroll-left1", "scroll-left2"});

	return config;
}

const const_image_button_appearance &
image_button_appearance_base::date_popup_right_theme()
{
	static const const_image_button_appearance config=
		create_date_popup_button({"scroll-right1", "scroll-right2"});

	return config;
}

LIBCXXW_NAMESPACE_END
