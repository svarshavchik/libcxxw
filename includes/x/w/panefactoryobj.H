/*
** Copyright 2018-2021 Double Precision, Inc.
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
#include <x/w/uielementsfwd.H>
#include <x/w/uigeneratorsfwd.H>

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

	//! Configure this pane for a new list or table.

	//! Replaces appearance with pane_appearance::base::focusable_list(),

	void configure_for_new_list();

	//! Configure a new list or a table in the pane

	//! Calls configure_for_new-list(), then calls the
	//! new_listlayoutmanager's configure_for_pane().

	void configure_new_list(new_listlayoutmanager &,
				bool synchronized=false);
	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;
};

LIBCXXW_NAMESPACE_END

#endif
