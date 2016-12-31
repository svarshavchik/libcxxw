/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_h
#define x_w_main_window_h

#include <x/w/main_windowfwd.H>
#include <x/w/main_windowobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for \ref main_window "main application window"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::main_window::base

class main_windowBase {

public:

	static main_window create();

	//! The custom create() for main window objects.

	template<typename T> class objfactory {
	public:

		template<typename ref_type, typename ...Args>
		static inline ref_type create(Args && ...args)
		{
			return main_windowBase::create(std::forward<Args>
						       (args)...);
		}
	};
};

LIBCXXW_NAMESPACE_END

#endif
