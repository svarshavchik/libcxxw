/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef booklayoutmanager_impl_h
#define booklayoutmanager_impl_h

#include "x/w/booklayoutmanager.H"
#include "x/w/pagelayoutmanagerfwd.H"
#include "x/w/pagelayoutmanagerobj.H"
#include "x/w/focusable.H"
#include "x/w/callback_triggerfwd.H"
#include "booklayoutmanager/bookgridlayoutmanager.H"
#include "x/w/pagetabgridlayoutmanagerfwd.H"
#include "booklayoutmanager/pagetabgridlayoutmanagerobj.H"
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

	//! And the underlying page layout manager implementation object.
	const ref<pagelayoutmanagerObj::implObj> book_pagelayoutmanager_impl;

	//! The underlying pagelayoutmanager

	pagelayoutmanager book_pagelayoutmanager();

	//! The container with the underlying pagetabgridlayoutmanager

	const container book_pagetabgrid;

	//! And the underlying page tab grid layout manager implementation object.
	const ref<pagetabgridlayoutmanagerObj::implObj
		  > book_pagetabgridlayoutmanager_impl;

	//! The underlying pagetabgridlayoutmanager
	pagetabgridlayoutmanager book_pagetabgridlayoutmanager();

	//! Constructor
	implObj(const ref<bookgridlayoutmanagerObj> &impl);

 private:

	//! Delegated constructor

	implObj(const ref<bookgridlayoutmanagerObj> &impl,
		grid_map_t::lock &&grid_lock);
 public:
	//! Destructor
	~implObj();

	//! Return the given page tab.

	static pagetabptr get_pagetab(grid_map_t::lock &lock, size_t);

	//! Return the given page tab.

	static pagetabptr get_pagetab(book_lock &lock, size_t);

	//! Return all the focusables

	//! Returns the left scroll button, the tabs, and the right scroll
	//! button.

	std::vector<focusable> get_focusables() const;

	//! Return the main focusable.

	//! Returns the first tab. If there are no tabs, returns the
	//! left scroll button as the main focusable.

	focusable get_focusable() const;

	//! After a new tab is added, we need to rebuild the focusable order.

	void rebuild_focusables() const;
};

LIBCXXW_NAMESPACE_END

#endif
