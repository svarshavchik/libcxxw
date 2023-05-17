/*
** Copyright 2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uicompiler_list_items_h
#define x_w_uicompiler_list_items_h

#include "uicompiler.H"
#include "x/w/image_param.H"
#include "x/w/shortcut.H"
#include "x/w/uigenerators.H"

LIBCXXW_NAMESPACE_START

//! Helper object when parsing new list items from the theme file.

struct uicompiler::list_items_parse_info_t {

	vector<functionref<void (std::vector<list_item_param> &,
				 uielements &elements)>> &params;

	vector<std::string> &labels;

	void name(const std::string &) const;

	void label(const text_param &) const;

	void image(const std::string &name) const;

	void add_separator() const;

	void add_shortcut(const shortcut &sc) const;

	void add_inactive_shortcut(const shortcut &sc) const;

	void add_hierindent(size_t) const;

	void add_menuoption(const std::string &) const;

	void add_selected() const;

	void add_submenu(const listlayoutmanager_generator &gens) const;

	void add_status_change(const std::string &name) const;
};


LIBCXXW_NAMESPACE_END

#endif
