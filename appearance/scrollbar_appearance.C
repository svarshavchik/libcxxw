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

const_scrollbar_appearance scrollbar_appearanceObj
::do_modify(const function<void (const scrollbar_appearance &)> &closure) const
{
	auto copy=scrollbar_appearance::create(*this);
	closure(copy);
        return copy;
}

const const_scrollbar_appearance &scrollbar_appearance_base::theme()
{
	static const const_scrollbar_appearance config=
		const_scrollbar_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
