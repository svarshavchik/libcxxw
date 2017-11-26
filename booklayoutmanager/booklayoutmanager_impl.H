/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef booklayoutmanager_impl_h
#define booklayoutmanager_impl_h

#include "x/w/booklayoutmanager.H"
#include "x/w/pagelayoutmanagerfwd.H"
#include "x/w/focusable.H"
#include "booklayoutmanager/bookgridlayoutmanager.H"
#include "booklayoutmanager/pagetabgridlayoutmanagerfwd.H"
#include "booklayoutmanager/pagetabfwd.H"
#include "image_button.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! Internal implementation object for the book layout manager.

//! This is constructed when the booklayoutmanager public gets constructed.
//! The booklayoutmanager object gets constructed for the purpose of
//! modifying its contents, so we might as well construct the
//! layout manager for the internal objects: the container with the
//! pagelayoutmanager, containing the pages in the book, and the
//! grid layout manager for the tabs in their peephole.

class LIBCXX_HIDDEN booklayoutmanagerObj::implObj : virtual public obj {

 public:

	//! The internal implementation object for the book layout manager.

	const ref<bookgridlayoutmanagerObj> impl;

	//! The left scroll button;

	const image_button left_scrollbutton;

	//! The right scroll button;

	const image_button right_scrollbutton;

	//! The container with the underlying pagelayoutmanager

	const container book_pagecontainer;

	//! The underlying pagelayoutmanager

	const pagelayoutmanager book_pagelayoutmanager;

	//! The container with the underlying pagetabgridlayoutmanager

	const container book_pagetabgrid;

	//! The underlying pagetabgridlayoutmanager

	const pagetabgridlayoutmanager book_pagetabgridlayoutmanager;

	//! Constructor
	implObj(const ref<bookgridlayoutmanagerObj> &impl);

	//! Destructor
	~implObj();

	//! Return the given pagetag

	pagetabptr get_pagetab(size_t) const;

	//! Return all the focusables

	//! Returns the left scroll button, the tabs, and the right scroll
	//! button.
	//!
	//! A book_lock must be instantatiated first. The parameter is
	//! ignored, this is contract enforcement.
	std::vector<focusable> get_focusables(grid_map_t::lock &) const;

	//! After a new tab is added, we need to rebuild the focusable order.

	void rebuild_focusables(grid_map_t::lock &) const;

	//! Open a page.

	static void open(const const_booklayoutmanager &blm, size_t n,
			 bool ensure_visibility);

};

LIBCXXW_NAMESPACE_END

#endif
