/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/list_appearance.H"
#include "x/w/scrollbar_appearance.H"

LIBCXXW_NAMESPACE_START

list_appearance_properties::list_appearance_properties()
	: list_border{"list_border"},
	  focusoff_border{"listfocusoff_border"},
	  focuson_border{"listfocuson_border"},
	  v_padding{"list_v_padding"},
	  h_padding{"list_h_padding"},
	  indent{"list_indent"},
	  background_color{"list_background_color"},
	  selected_color{"list_selected_color"},
	  highlighted_color{"list_highlighted_color"},
	  current_color{"list_current_color"},
	  list_foreground_color{"label_foreground_color"},
	  list_font{theme_font{"list"}},
	  shortcut_foreground_color{"label_foreground_color"},
	  shortcut_font{theme_font{"menu_shortcut"}},
	  list_separator_border{"list_separator_border"},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
{
}

list_appearance_properties::~list_appearance_properties()=default;

void list_appearance_properties::visible_focusoff_border()
{
	focusoff_border="listvisiblefocusoff_border";
}

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

const const_list_appearance &list_appearance_base::theme()
{
	static const const_list_appearance obj=list_appearance::create();

	return obj;
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

const const_list_appearance &list_appearance_base::list_pane_theme()
{
	static const const_list_appearance obj=create_list_pane_theme();

	return obj;
}

static auto create_table_theme()
{
	auto c=list_appearance::base::theme()
		->modify([]
			 (const auto &c)
			 {
				 c->visible_focusoff_border();
			 });

	return c;
}

const const_list_appearance &list_appearance_base::table_theme()
{
	static const const_list_appearance obj=create_table_theme();

	return obj;
}

static auto create_table_pane_theme()
{
	auto c=list_appearance::base::list_pane_theme()
		->modify([]
			 (const auto &c)
			 {
				 c->visible_focusoff_border();
			 });

	return c;
}

const const_list_appearance &list_appearance_base::table_pane_theme()
{
	static const const_list_appearance obj=create_table_pane_theme();

	return obj;
}

LIBCXXW_NAMESPACE_END
