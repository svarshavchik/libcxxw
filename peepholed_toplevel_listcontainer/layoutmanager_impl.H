/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_popup_layoutmanager_h
#define custom_combobox_popup_layoutmanager_h

#include "listlayoutmanager/listlayoutmanager.H"
#include "peepholed_toplevel_listcontainer/impl.H"

LIBCXXW_NAMESPACE_START

//! Combo-box popup layout manager implementation.

//! This is a listlayoutmanagerObj::implObj, except that
//! process_updated_position() is overridden in order to capture the height
//! of the largest row, which is used to set the height of the combo-box
//! current selection element (so that it's tall enough to show any
//! combo-box selection).

class LIBCXX_HIDDEN peepholed_toplevel_listcontainer_layoutmanager_implObj
	: public listlayoutmanagerObj::implObj {

 public:

	//! My container
	const ref<peepholed_toplevel_listcontainer_implObj> container_impl;

	//! Constructor
	peepholed_toplevel_listcontainer_layoutmanager_implObj
		(const ref<peepholed_toplevel_listcontainer_implObj
		 > &container_impl,
		 const ref<listcontainerObj::implObj> &listcontainer_impl,
		 const new_listlayoutmanager &style);

	//! Destructor
	~peepholed_toplevel_listcontainer_layoutmanager_implObj();

	//! Override process_updated_position.

	//! Determine the tallest row's height, and call
	//! container_impl->update_tallest_row_height().
	void process_updated_position(IN_THREAD_ONLY,
				      const rectangle &position) override;

};

LIBCXXW_NAMESPACE_END

#endif
