/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_appearance_h
#define x_w_list_appearance_h

#include <x/w/list_appearancefwd.H>
#include <x/w/list_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref list_appearance "list appearance object".

class list_appearance_base : public ptrref_base {

public:

	//! Return the default theme-specified list appearance.

	static const const_list_appearance &theme();

	//! Return the default theme-specified list appearance in panes

	static const const_list_appearance &pane_theme();

	//! Return the default theme-specified list appearance in tables

	static const const_list_appearance &table_theme();
};

LIBCXXW_NAMESPACE_END

#endif
