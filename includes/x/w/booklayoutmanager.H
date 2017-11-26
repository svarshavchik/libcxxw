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
#include <x/w/rgb.H>
#include <x/w/border_arg.H>
#include <x/w/callback_triggerfwd.H>

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

	//! Page background color;
	color_arg background_color;

	//! Border
	border_arg border;

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

//! Parameters passed to a book layout manager's on_opened() callback.

//! \see booklayoutmanager

struct LIBCXX_PUBLIC book_status_info_t {

	//! The lock on the book layout manager.

	//! The layout manager itself can be fished out of the lock.

	const book_lock &lock;

	//! Which page of the book is now open.

	size_t opened;

	//! Why it was opened

	const callback_trigger_t &trigger;

	//! A busy mcguffin, if needed.
	const busy &mcguffin;
};

LIBCXXW_NAMESPACE_END

#endif
