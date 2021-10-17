/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarlayoutmanagerobj_h
#define x_w_menubarlayoutmanagerobj_h

#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/menubarfactoryfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/uielementsfwd.H>
#include <x/w/uigeneratorsfwd.H>
#include <x/w/menufwd.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! Do not use grid layout manager's methods directly. Only use methods
//! defined for the menu bar layout manager.
//!
//! \see menubarlayoutmanager

class menubarlayoutmanagerObj : public gridlayoutmanagerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menubarlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~menubarlayoutmanagerObj() LIBCXX_HIDDEN;

	//! How many menus there are.
	size_t menus() const;

	//! How many menus there are in the right section.
	size_t right_menus() const;

	//! Return the given menu
	menu get_menu(size_t n) const;

	//! Return the given menu
	menu get_right_menu(size_t n) const;

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

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
