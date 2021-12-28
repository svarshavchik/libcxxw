/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_h
#define x_w_main_window_h

#include <x/w/main_windowfwd.H>
#include <x/w/main_windowobj.H>
#include <x/w/main_window_appearancefwd.H>
#include <x/w/screen_positions.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/generic_window.H>
#include <x/w/text_param.H>
#include <x/w/stop_message.H>
#include <x/w/alert_message.H>
#include <x/w/border_arg.H>
#include <x/w/font_arg.H>
#include <x/w/rgb.H>
#include <x/w/rectangle.H>
#include <x/w/concepts.H>
#include <x/w/screen_positionsfwd.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <functional>
#include <optional>

LIBCXXW_NAMESPACE_START

struct main_window_position {

	//! The name of this window.

	const std::string name="main";

	//! Wrapper for screen_positions

	//! Avoids the need to include screen_positions header file.

	struct screen_positions_t : screen_positions {

		//! Default constructor

		screen_positions_t();

		//! Explicit constructor
		screen_positions_t(const screen_positions &);

		//! Destructor

		~screen_positions_t();

		//! Copy constructor

		screen_positions_t(const screen_positions_t &);

		//! Assignment operator

		screen_positions_t &operator=(const screen_positions_t &);

		using screen_positions::operator=;

		using screen_positions::operator->;
	};

	//! Saved positions of windows and widgets

	screen_positions_t positions;
};

//! Optional configuration of a \ref main_window "main application window".

struct main_window_config : main_window_position {

public:
	//! Wrapper for the appearance object.

	struct appearance_t : const_main_window_appearance {

		//! Default constructor

		appearance_t();

		//! Destructor

		~appearance_t();

		//! Copy constructor

		appearance_t(const appearance_t &);

		//! Assignment operator

		appearance_t &operator=(const appearance_t &);

		using const_main_window_appearance::const_main_window_appearance
		;
		using const_main_window_appearance::operator=;

		using const_main_window_appearance::operator->;
	};

	//! Appearance
	appearance_t appearance;

	//! Destructor
	~main_window_config();
};

//! Create a splash window instead of a regular \ref main_window "main_window".

//! Alternative optional configuration of a main_window that creates a splash
//! window, with no title, automatically centered, and without a window
//! title or any normal decorations.

class splash_window_config : public main_window_config {

public:

	//! Splash window's border.
	border_arg border=default_border();

	//! Destructor
	~splash_window_config();

	//! Default border for splash window.
	static border_arg default_border();
};

//! Creates a splash window with a rounded border.

//! An alternative to a \ref splash_window_config "splash_window_config" that
//! uses a slightly rounded border, for a more modern appearance.
//!
//! Only works on displays that support alpha channels, otherwise this
//! produces a regular splash window with square borders.
//!
//! The resulting main_window should not have its background set manually
//! via set_background_color(). Instead, the background_color that's
//! inherited from splash_window_config sets the initial background color
//! inside the borders, and it should be left at that.

class transparent_splash_window_config : public splash_window_config {

public:

	//! Border for the splash window.
	border_arg border=default_border();

	//! Destructor
	~transparent_splash_window_config();

	//! Default border.
	static border_arg default_border();
};

//! Base class for \ref main_window "main application window"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::main_window::base

class main_windowBase {

public:

	//! Creator for create_mainwindow()

	typedef void main_window_creator_t(const main_window &);

	//! Create a new top level window, using the default layout manager

	template<main_window_processor functor>
	static main_window create(functor &&f)
	{
		return do_create(make_function<main_window_creator_t>
				 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<main_window_processor functor>
	static main_window create(functor &&f,
				  const new_layoutmanager &layout_factory)
	{
		return do_create(make_function<main_window_creator_t>
				 (std::forward<functor>(f)),
				 layout_factory);
	}

	//! Create a new top level window, using the default layout manager

	template<main_window_processor functor>
	static main_window create(const main_window_config_t &config,
				  functor &&f)
	{
		return do_create(config,
				 make_function<main_window_creator_t>
				 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<main_window_processor functor>
	static main_window create(const main_window_config_t &config,
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

	static main_window do_create(const main_window_config_t &config,
				     const function<main_window_creator_t> &f)
		LIBCXX_PUBLIC;

	//! create() implementation.

	static main_window do_create(const main_window_config_t &config,
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
