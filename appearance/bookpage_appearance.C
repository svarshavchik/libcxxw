/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/bookpage_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

bookpage_appearance_properties::bookpage_appearance_properties()
	: noncurrent_color{"book_tab_inactive_color"},
	  current_color{"book_tab_active_color"},
	  warm_color{"book_tab_warm_color"},
	  active_color{"book_tab_hot_color"},
	  label_font{theme_font{"book_tab_font"}},
	  label_foreground_color{"book_tab_font_color"},
	  horiz_padding{"book_tab_h_padding"},
	  vert_padding{"book_tab_v_padding"}
{
}

bookpage_appearance_properties::~bookpage_appearance_properties()=default;

bookpage_appearanceObj::bookpage_appearanceObj()=default;

bookpage_appearanceObj::~bookpage_appearanceObj()=default;

bookpage_appearanceObj::bookpage_appearanceObj
(const bookpage_appearanceObj &o)
	: bookpage_appearance_properties{o}
{
}

const_bookpage_appearance bookpage_appearanceObj
::do_modify(const function<void (const bookpage_appearance &)> &closure) const
{
	auto copy=bookpage_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct bookpage_appearance_base_themeObj : virtual public obj {

	const const_bookpage_appearance config=const_bookpage_appearance::create();

};

#if 0
{
#endif
}

const_bookpage_appearance bookpage_appearance_base::theme()
{
	return singleton<bookpage_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
