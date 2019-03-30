/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/tooltip_appearance.H"
#include "x/w/generic_window_appearance.H"

LIBCXXW_NAMESPACE_START

void tooltip_border::set_theme_border(const std::string &border)
{
	this->border=border;
	this->hpad=border + "_padding_h";
	this->vpad=border + "_padding_v";
}

static inline tooltip_border default_alpha_border()
{
	tooltip_border b;

	b.set_theme_border("tooltip_border");

	return b;
}

static inline tooltip_border default_nonalpha_border()
{
	tooltip_border b;

	b.set_theme_border("tooltip_border_square");
	return b;
}

tooltip_appearance_properties::tooltip_appearance_properties()
	: alpha_border{default_alpha_border()},
	  nonalpha_border{default_nonalpha_border()},
	  tooltip_background_color{"tooltip_background_color"},
	  toplevel_appearance{generic_window_appearance::base::tooltip_theme()},
	  tooltip_x_offset{"tooltip_x_offset"},
	  tooltip_y_offset{"tooltip_y_offset"}
{
}

tooltip_appearance_properties::~tooltip_appearance_properties()=default;

tooltip_appearanceObj::tooltip_appearanceObj()=default;

tooltip_appearanceObj::~tooltip_appearanceObj()=default;

tooltip_appearanceObj::tooltip_appearanceObj
(const tooltip_appearanceObj &o)
	: tooltip_appearance_properties{o}
{
}

const_tooltip_appearance tooltip_appearanceObj
::do_modify(const function<void (const tooltip_appearance &)> &closure) const
{
	auto copy=tooltip_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_tooltip_appearance &tooltip_appearance_base::tooltip_theme()
{
	static const const_tooltip_appearance config=
		const_tooltip_appearance::create();

	return config;
}

const const_tooltip_appearance &tooltip_appearance_base::static_tooltip_theme()
{
	static const const_tooltip_appearance config=
		tooltip_theme()->modify
		([]
		 (const auto &theme)
		 {
			 theme->tooltip_background_color=
				 "static_tooltip_background_color";
		 });

	return config;
}

LIBCXXW_NAMESPACE_END
