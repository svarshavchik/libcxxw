/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/list_appearance.H"
#include "x/w/generic_window_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

list_appearance_properties::list_appearance_properties()
	: list_border{"list_border"},
	  focus_border{focus_border_appearance::base::list_theme()},
	  v_padding{"list_v_padding"},
	  h_padding{"list_h_padding"},
	  indent{"list_indent"},
	  background_color{"list_background_color"},
	  selected_color{"list_selected_color"},
	  highlighted_color{"list_highlighted_color"},
	  current_color{"list_current_color"},
	  unmarked_icon{"bullet1"},
	  marked_icon{"bullet2"},
	  submenu_icon{"submenu"},
	  contents_appearance{generic_window_appearance::base::list_contents_theme()},
	  shortcut_foreground_color{"label_foreground_color"},
	  shortcut_font{theme_font{"menu_shortcut"}},
	  list_separator_border{"list_separator_border"},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
{
}

list_appearance_properties::~list_appearance_properties()=default;

list_appearanceObj::list_appearanceObj()=default;

list_appearanceObj::list_appearanceObj(const list_appearanceObj &orig)
	: list_appearance_properties{orig}
{
}

list_appearanceObj::~list_appearanceObj()=default;

const_list_appearance list_appearanceObj
::do_modify(const function<void (const list_appearance &)> &closure) const
{
	auto copy=list_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct list_appearance_base_themeObj : virtual public obj {

	const const_list_appearance config=list_appearance::create();
};

#if 0
{
#endif
}

const_list_appearance list_appearance_base::theme()
{
	return singleton<list_appearance_base_themeObj>::get()->config;
}

static auto create_list_pane_theme()
{
	auto c=list_appearance::base::theme()
		->modify([]
			 (const auto &c)
			 {
				 c->list_border={};
			 });

	return c;
}

namespace {
#if 0
}
#endif

struct list_appearance_base_list_pane_themeObj : virtual public obj {

	const const_list_appearance config=create_list_pane_theme();
};

#if 0
{
#endif
}

const_list_appearance list_appearance_base::list_pane_theme()
{
	return singleton<list_appearance_base_list_pane_themeObj>::get()->config;
}

static auto create_table_theme()
{
	auto c=list_appearance::base::theme()
		->modify([]
			 (const auto &c)
			 {
				 c->focus_border=focus_border_appearance
					 ::base::visible_list_theme();
			 });

	return c;
}

namespace {
#if 0
}
#endif

struct list_appearance_base_table_themeObj : virtual public obj {

	const const_list_appearance config=create_table_theme();
};

#if 0
{
#endif
}

const_list_appearance list_appearance_base::table_theme()
{
	return singleton<list_appearance_base_table_themeObj>::get()->config;
}

static auto create_table_pane_theme()
{
	auto c=list_appearance::base::list_pane_theme()
		->modify([]
			 (const auto &c)
			 {
				 c->focus_border=focus_border_appearance
					 ::base::visible_list_theme();
			 });

	return c;
}

namespace {
#if 0
}
#endif

struct list_appearance_base_table_pane_themeObj : virtual public obj {

	const const_list_appearance config=create_table_pane_theme();
};

#if 0
{
#endif
}

const_list_appearance list_appearance_base::table_pane_theme()
{
	return singleton<list_appearance_base_table_pane_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
