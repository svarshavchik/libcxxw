/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/table_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

table_appearance_properties::table_appearance_properties()
	: header_color{"list_header_color"},
	  adjustable_header_color{"list_adjustable_header_color"},
	  adjustable_header_highlight_color
	{"list_adjustable_header_highlight_color"},
	  adjustable_header_highlight_width
	{"list_adjustable_header_highlight_width"},
	  drag_horiz_start{"drag_horiz_start"},
	  adjust_cursor{"slider-horiz"}
{
}

table_appearance_properties::~table_appearance_properties()=default;

table_appearanceObj::table_appearanceObj()=default;

table_appearanceObj::~table_appearanceObj()=default;

table_appearanceObj::table_appearanceObj
(const table_appearanceObj &o)
	: table_appearance_properties{o}
{
}

const_table_appearance table_appearanceObj
::do_modify(const function<void (const table_appearance &)> &closure) const
{
	auto copy=table_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct table_appearance_base_themeObj : virtual public obj {

	const const_table_appearance config=const_table_appearance::create();

};

#if 0
{
#endif
}

const_table_appearance table_appearance_base::theme()
{
	return singleton<table_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
