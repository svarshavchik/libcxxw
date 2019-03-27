/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/pane_layout_appearance.H"

LIBCXXW_NAMESPACE_START

pane_layout_appearance_properties::pane_layout_appearance_properties()
	: border{"pane_border"},
	  slider{"pane_slider"},
	  slider_background_color{"pane_slider_background"},
	  slider_focusoff_border{"pane_slider_focusoff_border"},
	  slider_focuson_border{"pane_slider_focuson_border"}
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

const const_pane_layout_appearance &pane_layout_appearance_base::theme()
{
	static const const_pane_layout_appearance config=
		const_pane_layout_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
