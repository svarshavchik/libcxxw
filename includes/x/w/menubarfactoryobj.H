/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarfactoryobj_h
#define x_w_menubarfactoryobj_h

#include <x/w/menubarfactoryfwd.H>
#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/menufwd.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menubarfactory

class menubarfactoryObj : public obj {

public:

	//! Constructor
	menubarfactoryObj(const menubarlayoutmanager &layout);

	//! Destructor
	~menubarfactoryObj();

	//! The factory's layout manager.
	const menubarlayoutmanager layout;

	//! Creator callback signature.
	typedef void menu_creator_t(const factory &);

	//! Add a menubar item.

	//! The parameter is a callback that gets invoked, passing a factory
	//! object as the first parameter. The callback must use the factory
	//! to construct the new menubar item.

	template<typename functor>
	menu add(functor &&f)
	{
		return do_add(make_function<menu_creator_t>
			      (std::forward<functor>(f)));
	}

	//! Add a menu bar item that's plain text.
	menu add_text(const text_param &t);

	//! Add a menu bar item.

	virtual menu do_add(const function<menu_creator_t> &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
