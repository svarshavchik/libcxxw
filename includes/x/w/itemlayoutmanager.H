/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_itemlayoutmanager_h
#define x_w_itemlayoutmanager_h

#include <x/w/itemlayoutmanagerfwd.H>
#include <x/w/itemlayoutmanagerobj.H>
#include <x/w/item_layout_appearancefwd.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

/*! Create a list of items.

Passed to create_focusable_container() to create a focusable container
with an \ref itemlayoutmanager "item layout manager".

*/

class new_itemlayoutmanager : public new_focusable_layoutmanager {

public:
	//! Constructor
	new_itemlayoutmanager() LIBCXX_PUBLIC;

	//! Constructor
	new_itemlayoutmanager(const itemlayout_callback_t &callback)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_itemlayoutmanager() LIBCXX_PUBLIC;

	//! Copy constructor
	new_itemlayoutmanager(const new_itemlayoutmanager &);

	//! Assignment operator
	new_itemlayoutmanager &operator=(const new_itemlayoutmanager &);

	//! Callback that gets invoked when the close button is clicked.
	itemlayout_callback_t callback;

	//! Visual appearance of the item layout manager.
	const_item_layout_appearance appearance;

	//! Create a new container that uses the item layout manager

	//! \internal
	focusable_container create(const container_impl &parent,
				   const function<void
				   (const focusable_container &)>
				   &creator)
		const override LIBCXX_HIDDEN;
};

//! Acquire a lock on the items managed by the layout manager.

//! \see itemlayoutmanager

class itemlayout_lock :
	public mpobj_lock<itemlayout_button_container,
			  std::recursive_mutex> {

public:
	//! The locked layout manager.

	const itemlayoutmanager layout_manager;

	//! Constructor
	itemlayout_lock(const itemlayoutmanager &);

	//! Destructor
	~itemlayout_lock();
};

LIBCXXW_NAMESPACE_END

#endif
