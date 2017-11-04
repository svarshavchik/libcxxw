/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarfactoryobj_h
#define x_w_menubarfactoryobj_h

#include <x/w/menubarfactoryfwd.H>
#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
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

	//! Menu creator callback signature.
	typedef void menu_creator_t(const factory &);

	//! Menu content creator callback signature.

	typedef void menu_content_creator_t(const listlayoutmanager &);

	//! Add a menubar item.

	//! The first parameter is a callback that gets invoked, passing a
	//! factory
	//! object as the first parameter. The callback must use the factory
	//! to construct the new menubar item.
	//!
	//! The second parameter is the creator for the menu's contents,
	//! that receives a \ref menulayoutmanager "menu layout manager"
	//! as its parameter.

	template<typename functor, typename content_functor>
	menu add(functor &&f, content_functor &&cf)
	{
		return do_add(make_function<menu_creator_t>
			      (std::forward<functor>(f)),
			      make_function<menu_content_creator_t>
			      (std::forward<content_functor>(cf)));
	}

	//! Add a menu bar item that's plain text.
	template<typename content_functor>
	menu add_text(const text_param &t, content_functor &&cf)
	{
		return do_add_text(t,
				   make_function<menu_content_creator_t>
				   (std::forward<content_functor>(cf)));
	}

	//! Add a menu bar item that's plain text.
	menu do_add_text(const text_param &t,
			 const function<menu_content_creator_t> &cf);

	//! Add a menu bar item.

	virtual menu do_add(const function<menu_creator_t> &,
			    const function<menu_content_creator_t> &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
