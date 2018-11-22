/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_pseudo_impl_h
#define listcontainer_pseudo_impl_h

#include "listlayoutmanager/listcontainer_pseudo_implfwd.H"
#include "listlayoutmanager/list_elementfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/containerobj.H"
#include "x/w/impl/themedimfwd.H"

LIBCXXW_NAMESPACE_START

//! The pseudo-container with a \ref listlayoutmanager "list layout manager".

//! Base class for a container that uses the list layout manager.
//!
//! The container has one element, the \ref list_element "list element", that
//! draws the items in the list. This container makes it possible to use
//! the layout manager framework for managing list items, even though they
//! are not, themselves, discrete display elements.

class LIBCXX_HIDDEN listcontainer_pseudo_implObj :
	public container_elementObj<child_elementObj> {

 public:

	//! Alias
	typedef container_elementObj<child_elementObj> superclass_t;

	//! Constructor
	listcontainer_pseudo_implObj(const container_impl &parent);

	//! Destructor
	~listcontainer_pseudo_implObj();

	//! Number of rows in the list.
	size_t rows(ONLY IN_THREAD) const;

	//! The height of the tallest row.
	tallest_row_height_t rowsize(ONLY IN_THREAD) const;

	//! Override do_draw().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas) override;
 private:

	//! do_draw() saves the visible height of the list contanier.
	dim_t most_recent_visible_height_thread_only;
 public:
	THREAD_DATA_ONLY(most_recent_visible_height);
};

LIBCXXW_NAMESPACE_END

#endif
