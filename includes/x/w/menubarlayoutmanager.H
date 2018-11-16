/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarlayoutmanager_h
#define x_w_menubarlayoutmanager_h

#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/menubarlayoutmanagerobj.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/factory.H>
#include <x/w/menu.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! A lock on the \ref menubarlayoutmanager "menu bar".

//! The actual lock is gridlayoutmanagerObj::grid_lock.

class menubar_lock {

public:
	//! The locked menu bar.

	const menubarlayoutmanager manager;

	//! Constructor
	menubar_lock(const menubarlayoutmanager &manager);

	//! Destructor
	~menubar_lock();

	//! How many menus there are.
	size_t menus() const;

	//! How many menus there are in the right section.
	size_t right_menus() const;

	//! Return the given menu
	menu get_menu(size_t n) const;

	//! Return the given menu
	menu get_right_menu(size_t n) const;
};

LIBCXXW_NAMESPACE_END

#endif
