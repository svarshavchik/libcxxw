/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/font_picker_appearance.H"
#include "x/w/combobox_appearance.H"
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
				  }}
{
}

font_picker_appearance_properties::~font_picker_appearance_properties()=default;

font_picker_appearanceObj::font_picker_appearanceObj()=default;

font_picker_appearanceObj::~font_picker_appearanceObj()=default;

font_picker_appearanceObj::font_picker_appearanceObj
(const font_picker_appearanceObj &o)
	: font_picker_appearance_properties{o},
	  element_popup_config{o}
{
}

font_picker_appearance font_picker_appearanceObj::clone() const
{
	return font_picker_appearance::create(*this);
}

const const_font_picker_appearance &font_picker_appearance_base::theme()
{
	static const const_font_picker_appearance config=
		const_font_picker_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
