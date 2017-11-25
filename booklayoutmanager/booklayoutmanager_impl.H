/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef booklayoutmanager_impl_h
#define booklayoutmanager_impl_h

#include "x/w/booklayoutmanager.H"
#include "x/w/pagelayoutmanagerfwd.H"
#include "booklayoutmanager/bookgridlayoutmanager.H"
#include "booklayoutmanager/pagetabgridlayoutmanagerfwd.H"
#include "booklayoutmanager/pagetabfwd.H"

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

	//! The container with the underlying pagelayoutmanager

	const container book_switchcontainer;

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
};

LIBCXXW_NAMESPACE_END

#endif
