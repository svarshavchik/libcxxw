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
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menubarlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~menubarlayoutmanagerObj() LIBCXX_HIDDEN;

	menubarfactory append_menus();

	menubarfactory insert_menus(size_t pos);

	menubarfactory append_right_menus();

	menubarfactory insert_right_menus(size_t pos);

	void remove_menu(size_t pos);

	void remove_menu_right(size_t pos);
};

LIBCXXW_NAMESPACE_END

#endif
