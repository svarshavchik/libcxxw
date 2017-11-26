/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef bookgridlayoutmanager_impl_h
#define bookgridlayoutmanager_impl_h

#include "gridlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! The actual layout manager for a container with a \ref booklayoutmanager "booklayoutmanager"

//! The container uses the grid layout manager to position the pane, that
//! uses the page layout manager, and the arrows and the peephole with
//! the book tabs on the first row.


class LIBCXX_HIDDEN bookgridlayoutmanagerObj
	: public gridlayoutmanagerObj::implObj {


	std::function<void (const book_status_info_t &)> callback_under_lock;

 public:
	//! Constructor
	bookgridlayoutmanagerObj(const ref<containerObj::implObj> &);

	//! Destructor
	~bookgridlayoutmanagerObj();

	//! Override create_public_object()

	//! Returns a booklayoutmanager
	layoutmanager create_public_object() override;

	//! The callback, accessible only while holding a lock.

	inline auto &callback(book_lock &)
	{
		return callback_under_lock;
	}
};

LIBCXXW_NAMESPACE_END

#endif
