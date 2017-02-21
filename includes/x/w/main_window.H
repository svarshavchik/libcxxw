/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_h
#define x_w_main_window_h

#include <x/w/main_windowfwd.H>
#include <x/w/main_windowobj.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/ref.H>
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref main_window "main application window"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::main_window::base

class main_windowBase {

public:

	//! Creator for create_mainwindow()

	typedef void main_window_creator_t(const main_window &);

	//! Create a new top level window, using the mainwindowlayout layout manager

	template<typename functor>
	static main_window create(functor &&f)
	{
		return do_create(make_function<void (const main_window &w)>
				 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<typename functor>
	static main_window create(const new_layoutmanager &layout_factory,
				  functor &&f)
	{
		return do_create(layout_factory,
				 make_function<main_window_creator_t>
				 (std::forward<functor>(f)));
	}

	//! create() implementation.

	static main_window do_create(const function<main_window_creator_t> &f);

	//! create() implementation.

	static main_window do_create(const new_layoutmanager &actory,
				     const function<main_window_creator_t> &f);

	//! The custom create() for main window objects.

	template<typename T> class objfactory {
	public:

		template<typename ...Args>
		static inline T create(Args && ...args)
		{
			return main_windowBase::create(std::forward<Args>
						       (args)...);
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
