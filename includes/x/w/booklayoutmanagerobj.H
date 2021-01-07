/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_booklayoutmanagerobj_h
#define x_w_booklayoutmanagerobj_h

#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/bookpagefactoryfwd.H>
#include <x/w/pagelayoutmanagerfwd.H>
#include <x/w/pagetabgridlayoutmanagerfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/callback_triggerfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see booklayoutmanager

class booklayoutmanagerObj : public layoutmanagerObj {

public:

	//! Internal page layout manager.

	//! Do not access the internal page layout manager, only use
	//! book layout manager's methods.
	//!
	//! \internal
	const pagelayoutmanager book_pagelayoutmanager;

	//! Internal page tab grid layout manager.

	//! Do not access the internal page layout manager, only use
	//! book layout manager's methods.
	//!
	//! \internal

	const pagetabgridlayoutmanager book_pagetabgridlayoutmanager;

	class LIBCXX_HIDDEN implObj;

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

	//! Remove a page
	void remove(size_t n);

	//! How many pages in the book.
	size_t pages() const;

	//! Open the book to the given page.
	void open(size_t);

	//! Open the book to the given page.
	void open(ONLY IN_THREAD, size_t);

	//! Open the book to the given page.
	void open(ONLY IN_THREAD, size_t, const callback_trigger_t &);

	//! Close the book, don't show any pages.
	void close();

	//! Close the book, don't show any pages.
	void close(ONLY IN_THREAD);

	//! Which page is currently open.
	std::optional<size_t> opened() const;

	//! Return the specified page widget.
	element get_page(size_t) const;

	//! Install a new callback

	void on_opened(const functionref<void (THREAD_CALLBACK,
					       const book_status_info_t &)>&);

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
