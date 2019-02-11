/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagelayoutmanager_h
#define x_w_pagelayoutmanager_h

#include <x/w/pagelayoutmanagerfwd.H>
#include <x/w/pagelayoutmanagerobj.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_layoutmanager.H>

LIBCXXW_NAMESPACE_START

//! A lock on the contents of the \ref pagelayoutmanager "page layout manager".

//! Blocks other execution threads from accessing the contents of the
//! page layout manager.

class page_lock {

	//! Internal lock.
	mpobj_lock<page_layout_info_s, std::recursive_mutex> lock;

 public:

	//! Constructor
	page_lock(const pagelayoutmanager &);

	//! Destructor
	~page_lock();
};

//! Create a new container that uses the \ref pagelayoutmanager "page layout manager"

class new_pagelayoutmanager : public new_layoutmanager {


public:
	//! Constructor
	new_pagelayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_pagelayoutmanager() LIBCXX_PUBLIC;

	//! Create a container with the page layout manager.

	//! \internal

	layout_impl create(const container_impl &) const override LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
