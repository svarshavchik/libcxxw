/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_booklayoutmanager_h
#define x_w_booklayoutmanager_h

#include <x/w/booklayoutmanagerfwd.H>
#include <x/w/booklayoutmanagerobj.H>
#include <x/w/new_focusable_layoutmanagerfwd.H>
#include <x/w/layoutmanager.H>
#include <x/w/element.H>
#include <x/w/grid_map_infofwd.H>

LIBCXXW_NAMESPACE_START

//! Create a new container with a \ref booklayoutmanager "book layout manager".

//! This parameter gets passed to a \ref factory "factory"'s
//! \c create_container()
//! when creating a main window, or a container that
//! uses the book layout manager.

class LIBCXX_HIDDEN new_booklayoutmanager
	: public new_focusable_layoutmanager {
 public:
	//! Constructor
	new_booklayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_booklayoutmanager() LIBCXX_PUBLIC;

	//! Create a container with the book layout manager.

	//! \internal
	focusable_container
		create(const ref<containerObj::implObj> &) const override;
};

//! Lock the book layout manager.

//! Blocks modifications to the contents of the book.

class book_lock : public grid_map_t::lock {

public:

	//! My layout manager.
	const const_booklayoutmanager layout_manager;

	//! Constructor
	book_lock(const const_booklayoutmanager &layout_manager);

	//! Destructor
	~book_lock();
};

LIBCXXW_NAMESPACE_END

#endif
