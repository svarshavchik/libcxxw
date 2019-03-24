/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/scrollbar_appearance.H"

LIBCXXW_NAMESPACE_START

scrollbar_appearance_properties::scrollbar_appearance_properties()
	: background_color{"scrollbar_background_color"},
	  focusoff_border{"scrollbarfocusoff_border"},
	  focuson_border{"scrollbarfocuson_border"}
{
}

scrollbar_appearance_properties::~scrollbar_appearance_properties()=default;

scrollbar_appearanceObj::scrollbar_appearanceObj()=default;

scrollbar_appearanceObj::~scrollbar_appearanceObj()=default;

scrollbar_appearanceObj::scrollbar_appearanceObj
(const scrollbar_appearanceObj &o)
	: scrollbar_appearance_properties{o}
{
}

scrollbar_appearance scrollbar_appearanceObj::clone() const
{
	return scrollbar_appearance::create(*this);
}

const const_scrollbar_appearance &scrollbar_appearance_base::theme()
{
	static const const_scrollbar_appearance config=
		const_scrollbar_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
