/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarfactoryobj_h
#define x_w_menubarfactoryobj_h

#include <x/w/menubarfactoryfwd.H>
#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/popup_list_appearancefwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/menufwd.H>
#include <x/w/shortcutfwd.H>
#include <x/w/concepts.H>
#include <x/functional.H>
#include <x/optional_argsfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menubarfactory

class menubarfactoryObj : virtual public obj {

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

	//! Optional parameters when adding a menu
	typedef optional_argconstrefs<shortcut,
				      const_popup_list_appearance> extra_args_t;
	//! Add a menubar item.

	//! The first parameter is a callback that gets invoked, passing a
	//! factory
	//! object as the first parameter. The callback must use the factory
	//! to construct the new menubar item.
	//!
	//! The second parameter is the creator for the menu's contents,
	//! that receives a \ref listlayoutmanager "list layout manager"
	//! as its parameter.

	template<factory_processor functor,
		 listlayoutmanager_processor content_functor,
		 typename ...Args>
	menu add(functor &&f, content_functor &&cf, Args && ...args)
	{
		return do_add(make_function<menu_creator_t>
			      (std::forward<functor>(f)),
			      make_function<menu_content_creator_t>
			      (std::forward<content_functor>(cf)),
			      {
			       std::forward<Args>(args)...
			      });
	}

	//! Add a menu bar item that's plain text.
	template<listlayoutmanager_processor content_functor, typename ...Args>
	menu add_text(const text_param &t, content_functor &&cf,
		      Args && ...args)
	{
		return do_add_text(t,
				   make_function<menu_content_creator_t>
				   (std::forward<content_functor>(cf)),
				   {
				    std::forward<Args>(args)...
				   });
	}

	//! Type-erased add_text().
	menu do_add_text(const text_param &t,
			 const function<menu_content_creator_t> &cf,
			 const extra_args_t &args);

	//! Type-erased add()

	menu do_add(const function<menu_creator_t> &,
		    const function<menu_content_creator_t> &,
		    const extra_args_t &args);

	//! Type-erased add()

	virtual menu do_add_impl(const function<menu_creator_t> &,
				 const function<menu_content_creator_t> &,
				 const shortcut &,
				 const const_popup_list_appearance &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
