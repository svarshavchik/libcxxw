/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/element_popup_appearance.H"
#include <x/w/scrollbar_appearance.H>
#include <x/w/generic_window_appearance.H>

LIBCXXW_NAMESPACE_START

element_popup_appearance_properties::element_popup_appearance_properties()

	: element_border{"element_attached_border"},
	  popup_border{"element_attached_popup_border"},
	  popup_background_color{"element_attached_popup_background_color"},
	  popup_scrollbars_background_color
	{"element_attached_popup_background_color"},
	  button_focusoff_border
	{"element_attached_popup_button_focusoff_border"},
	  button_focuson_border{"element_attached_popup_button_focuson_border"},
	  toplevel_appearance{generic_window_appearance::base
			      ::main_window_theme()},
	  button_image1{"scroll-right1"},
	  button_image2{"scroll-right2"},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
{
}

element_popup_appearance_properties::~element_popup_appearance_properties()=default;

element_popup_appearanceObj::element_popup_appearanceObj()=default;

element_popup_appearanceObj::~element_popup_appearanceObj()=default;

element_popup_appearanceObj::element_popup_appearanceObj
(const element_popup_appearanceObj &o)
	: element_popup_appearance_properties{o}
{
}

const_element_popup_appearance element_popup_appearanceObj
::do_modify(const function<void (const element_popup_appearance &)> &closure) const
{
	auto copy=element_popup_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_element_popup_appearance &element_popup_appearance_base::theme()
{
	static const const_element_popup_appearance config=
		const_element_popup_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
