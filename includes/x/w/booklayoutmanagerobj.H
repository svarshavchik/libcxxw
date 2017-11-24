/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_booklayoutmanagerobj_h
#define x_w_booklayoutmanagerobj_h

#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/elementfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see booklayoutmanager

class booklayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	booklayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~booklayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append new pages
	bookpagefactory append();

	//! Insert new pages
	bookpagefactory insert(size_t n);

	//! How many pages in the book.
	size_t pages() const;

	//! Open the book to the given page.
	void open(size_t) const;

	//! Close the book, don't show any pages.
	void close() const;

	//! Which page is currently open.
	std::optional<size_t> opened() const;

	//! Return the specified page element.
	element get_page(size_t) const;
};

LIBCXXW_NAMESPACE_END

#endif
