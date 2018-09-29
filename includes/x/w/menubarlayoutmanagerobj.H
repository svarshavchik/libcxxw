/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarlayoutmanagerobj_h
#define x_w_menubarlayoutmanagerobj_h

#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/menubarfactoryfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menubarlayoutmanager

class menubarlayoutmanagerObj : public layoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menubarlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~menubarlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append new menus.

	menubarfactory append_menus();

	//! Insert new menus before an existing menu.

	menubarfactory insert_menus(size_t pos);

	//! Append new menus on the right edge of the menu bar.
	menubarfactory append_right_menus();

	//! Insert new menus before an existing menu on the right edge of th emenu bar.
	menubarfactory insert_right_menus(size_t pos);

	//! Remove the specified menu from the menu bar.

	void remove_menu(size_t pos);

	//! Remove the specified menu from the right edge of the menu bar.

	void remove_right_menu(size_t pos);
};

LIBCXXW_NAMESPACE_END

#endif
