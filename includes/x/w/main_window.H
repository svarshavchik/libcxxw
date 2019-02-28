/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_h
#define x_w_main_window_h

#include <x/w/main_windowfwd.H>
#include <x/w/main_windowobj.H>
#include <x/w/screen_positionsfwd.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/generic_window.H>
#include <x/w/text_param.H>
#include <x/w/stop_message.H>
#include <x/w/alert_message.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <functional>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Optional configuration of a \ref main_window "main application window".

class main_window_config {


public:

	//! Specify optional position on the screen.

	void screen_position(const screen_positions &,
			     const std::string_view &);

	//! Loaded screen position.

	std::optional<std::tuple<std::string,
				 std::reference_wrapper<const screen_positions>>
		      > saved_position;

	//! Main window's background color;
	color_arg background_color="mainwindow_background";

	//! Destructor
	~main_window_config();
};

//! Base class for \ref main_window "main application window"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::main_window::base

class main_windowBase {

public:

	//! Creator for create_mainwindow()

	typedef void main_window_creator_t(const main_window &);

	//! SFINAE selector for a creator functor.

	template<typename functor>
	using is_main_window_creator=
		std::void_t<decltype(std::declval<functor &&>()
				     (std::declval<const main_window &>
				     ()))>;
	//! Create a new top level window, using the default layout manager

	template<typename functor,
		 typename =is_main_window_creator<functor>>
	static main_window create(functor &&f)
	{
		return do_create(make_function<main_window_creator_t>
				 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<typename functor,
		 typename =is_main_window_creator<functor>>
	static main_window create(functor &&f,
				  const new_layoutmanager &layout_factory)
	{
		return do_create(make_function<main_window_creator_t>
				 (std::forward<functor>(f)),
				 layout_factory);
	}

	//! Create a new top level window, using the default layout manager

	template<typename functor,
		 typename =is_main_window_creator<functor>>
	static main_window create(const main_window_config &config,
				  functor &&f)
	{
		return do_create(config,
				 make_function<main_window_creator_t>
				 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<typename functor>
	static main_window create(const main_window_config &config,
				  functor &&f,
				  const new_layoutmanager &layout_factory)
	{
		return do_create(config,
				 make_function<main_window_creator_t>
				 (std::forward<functor>(f)),
				 layout_factory);
	}

	//! create() implementation.

	static main_window do_create(const function<main_window_creator_t> &f)
		LIBCXX_PUBLIC;

	//! create() implementation.

	static main_window do_create(const function<main_window_creator_t> &f,
				     const new_layoutmanager &factory)
		LIBCXX_PUBLIC;

	//! create() implementation.

	static main_window do_create(const main_window_config &config,
				     const function<main_window_creator_t> &f)
		LIBCXX_PUBLIC;

	//! create() implementation.

	static main_window do_create(const main_window_config &config,
				     const function<main_window_creator_t> &f,
				     const new_layoutmanager &factory)
		LIBCXX_PUBLIC;

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
