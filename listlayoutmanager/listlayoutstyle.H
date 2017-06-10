/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutstyle_h
#define listlayoutstyle_h

#include "x/w/listlayoutmanagerfwd.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "x/w/elementfwd.H"
#include "x/w/batch_queuefwd.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! Implements a particular list layout manager style.

class listlayoutstyle {

public:

	//! A temporary container for new list items.

	//! The list layout manager creates a new factory for the new row in
	//! the underlying list layout manager, and gives us a vector with
	//! each element for each list item column.

	typedef std::vector<element> new_list_items_t;

	//! Creating a new item.

	//! The list layout manager took care of collecting the raw
	//! elements that make up the new row in the list layout manager.
	//!
	//! We are expected to construct a container for each item that uses a
	//! listitemlayoutmanager.

	virtual void create_item(//! The list item layout manager
				 const ref<listlayoutmanagerObj::implObj> &lilm,

				 //! The grid factory, for creating the new item
				 const gridfactory &underlying_factory,

				 //! Queue for queueing recalculations
				 const batch_queue &queue,

				 //! The new elements for the list item
				 const new_list_items_t &new_list_items) const
	=0;
};

//! List layout style
LIBCXXW_NAMESPACE_END

#endif
