/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/tooltip_appearance.H"
#include "x/w/tooltip_border_appearance.H"
#include "x/w/generic_window_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

tooltip_appearance_properties::tooltip_appearance_properties()
	: alpha_border{tooltip_border_appearance::base::theme()},
	  nonalpha_border{tooltip_border_appearance::base::nonalpha_theme()},
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

namespace {
#if 0
}
#endif

struct tooltip_appearance_base_tooltip_themeObj : virtual public obj {

	const const_tooltip_appearance config=const_tooltip_appearance::create();

};

#if 0
{
#endif
}

const_tooltip_appearance tooltip_appearance_base::tooltip_theme()
{
	return singleton<tooltip_appearance_base_tooltip_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct tooltip_appearance_base_static_tooltip_themeObj : virtual public obj {

	const const_tooltip_appearance config=
		tooltip_appearance_base::tooltip_theme()->modify
		([]
		 (const auto &theme)
		 {
			 theme->tooltip_background_color=
				 "static_tooltip_background_color";
		 });

};

#if 0
{
#endif
}

const_tooltip_appearance tooltip_appearance_base::static_tooltip_theme()
{
	return singleton<tooltip_appearance_base_static_tooltip_themeObj>::get()->config;
}

namespace {
#if 0
}
#endif

struct tooltip_appearance_base_direction_tooltip_themeObj : virtual public obj {

	const const_tooltip_appearance config=
		tooltip_appearance_base::tooltip_theme()->modify
		([]
		 (const auto &theme)
		 {
			 theme->tooltip_background_color=
				 "direction_tooltip_background_color";
		 });

};

#if 0
{
#endif
}

const_tooltip_appearance tooltip_appearance_base::direction_tooltip_theme()
{
	return singleton<tooltip_appearance_base_direction_tooltip_themeObj>
		::get()->config;
}

LIBCXXW_NAMESPACE_END
