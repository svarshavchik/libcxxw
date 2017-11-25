/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef bookgridlayoutmanager_impl_h
#define bookgridlayoutmanager_impl_h

#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

//! The actual layout manager for a container with a \ref booklayoutmanager "booklayoutmanager"

//! The container uses the grid layout manager to position the pane, that
//! uses the page layout manager, and the arrows and the peephole with
//! the book tabs on the first row.


class LIBCXX_HIDDEN bookgridlayoutmanagerObj
	: public gridlayoutmanagerObj::implObj {

 public:
	//! Constructor
	bookgridlayoutmanagerObj(const ref<containerObj::implObj> &);

	//! Destructor
	~bookgridlayoutmanagerObj();

	//! Override create_public_object()

	//! Returns a booklayoutmanager
	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
