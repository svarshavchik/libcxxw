/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/main_window_appearance.H"
#include "x/w/focus_border_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/generic_window_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

main_window_appearance_properties::main_window_appearance_properties()
	: horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()},
	  background_color{"mainwindow_background"},
	  menubar_background_color{"menubar_background_color"},
	  toplevel_appearance{generic_window_appearance::base
			      ::main_window_theme()},
	  menu_highlighted_color{"menu_highlighted_color"},
	  menu_clicked_color{"menu_clicked_color"},
	  menu_focus_border{focus_border_appearance::base::menu_theme()},
	  menubar_border{"menubar_border"},
	  icon{"mainwindow-icon"},
	  icon_sizes{16, 24, 32, 48}
{
}

main_window_appearance_properties::~main_window_appearance_properties()=default;

main_window_appearanceObj::main_window_appearanceObj()=default;

main_window_appearanceObj::~main_window_appearanceObj()=default;

main_window_appearanceObj::main_window_appearanceObj
(const main_window_appearanceObj &o)
	: main_window_appearance_properties{o}
{
}

const_main_window_appearance main_window_appearanceObj
::do_modify(const function<void (const main_window_appearance &)> &closure) const
{
	auto copy=main_window_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct main_window_appearance_base_themeObj : virtual public obj {

	const const_main_window_appearance config=const_main_window_appearance::create();

};

#if 0
{
#endif
}

const_main_window_appearance main_window_appearance_base::theme()
{
	return singleton<main_window_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
