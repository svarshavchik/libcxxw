/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/font_picker_appearance.H"
#include "x/w/combobox_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/element_popup_appearance.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

font_picker_appearance_properties::font_picker_appearance_properties()
	: ok_label{_("Ok")},
	  cancel_label{_("Cancel")},
	  preview_width{"font_picker_preview_width"},
	  preview_height{"font_picker_preview_height"},
	  preview_border{"font_picker_preview_border"},
	  font_family_appearance{combobox_appearance::base::theme()},
	  font_size_appearance{combobox_appearance::base::theme()},
	  font_weight_appearance{combobox_appearance::base::theme()},
	  font_slant_appearance{combobox_appearance::base::theme()},
	  font_width_appearance{combobox_appearance::base::theme()},
	  unsupported_option_color{theme_color{
			  "font_picker_unavailable_option_color"
				  }},
	  preview_horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  preview_vertical_scrollbar{scrollbar_appearance::base::theme()},
	  attached_popup_appearance{const_element_popup_appearance::base
				    ::theme()}

{
}

font_picker_appearance_properties::~font_picker_appearance_properties()=default;

font_picker_appearanceObj::font_picker_appearanceObj()=default;

font_picker_appearanceObj::~font_picker_appearanceObj()=default;

font_picker_appearanceObj::font_picker_appearanceObj
(const font_picker_appearanceObj &o)
	: font_picker_appearance_properties{o}
{
}

const_font_picker_appearance font_picker_appearanceObj
::do_modify(const function<void (const font_picker_appearance &)> &closure)
	const
{
	auto copy=font_picker_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_font_picker_appearance &font_picker_appearance_base::theme()
{
	static const const_font_picker_appearance config=
		const_font_picker_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END