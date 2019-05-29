/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/pane_layout_appearance.H"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

pane_layout_appearance_properties::pane_layout_appearance_properties()
	: border{"pane_border"},
	  slider{"pane_slider"},
	  slider_background_color{"pane_slider_background"},
	  slider_focus_border{focus_border_appearance::base::slider_theme()},
	  slider_horiz{"slider-horiz"},
	  slider_vert{"slider-vert"}
{
}

pane_layout_appearance_properties::~pane_layout_appearance_properties()=default;

pane_layout_appearanceObj::pane_layout_appearanceObj()=default;

pane_layout_appearanceObj::~pane_layout_appearanceObj()=default;

pane_layout_appearanceObj::pane_layout_appearanceObj
(const pane_layout_appearanceObj &o)
	: pane_layout_appearance_properties{o}
{
}

const_pane_layout_appearance pane_layout_appearanceObj
::do_modify(const function<void (const pane_layout_appearance &)> &closure)
	const
{
	auto copy=pane_layout_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct pane_layout_appearance_base_themeObj : virtual public obj {

	const const_pane_layout_appearance config=const_pane_layout_appearance::create();

};

#if 0
{
#endif
}

const_pane_layout_appearance pane_layout_appearance_base::theme()
{
	return singleton<pane_layout_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
