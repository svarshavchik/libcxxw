/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_panefactoryobj_h
#define x_w_panefactoryobj_h

#include <x/w/panefactoryfwd.H>
#include <x/w/panelayoutmanagerfwd.H>
#include <x/w/pane_appearancefwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/factoryobj.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/w/alignment.H>

#include <x/mpobj.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see panefactory

class panefactoryObj : public factoryObj {

public:
	//! My layout manager.

	const panelayoutmanager layout;

	//! Appearance for the next pane
	const_pane_appearance appearance;

	//! Constructor
	panefactoryObj(const panelayoutmanager &layout) LIBCXX_HIDDEN;

	//! Destructor
	~panefactoryObj() LIBCXX_HIDDEN;

	//! Configure a new list or a table.

	//! Configure a new list for best visual appearance.
	//! Replaces appearance with pane_appearance::base::focusable_list(),
	//! and installs a new appearance theme in the list as well.

	void configure_new_list(new_listlayoutmanager &,
				bool synchronized=false);
};

LIBCXXW_NAMESPACE_END

#endif
