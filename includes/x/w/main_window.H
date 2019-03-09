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
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/functionalrefptrfwd.H>
#include <functional>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Main window appearance

struct main_window_colors {

	//! Main window's background color.
	color_arg background_color="mainwindow_background";

	//! Main window's menu bar's background color
	color_arg menubar_background_color="menubar_background_color";
};

//! Menu bar appearance

struct menu_button_colors {

	//! Menu button has input or pointer focus.
	color_arg menu_highlighted_color="menu_highlighted_color";

	//! Menu button is clicked.
	color_arg menu_clicked_color="menu_clicked_color";

	//! Menu button does not have keyboard focus
	border_arg menu_inputfocusoff_border="menu_inputfocusoff_border";

	//! Menu button has keyboard focus
	border_arg menu_inputfocuson_border="menu_inputfocuson_border";

	//! Border between the menu bar and the main window.
	border_arg menubar_border="menubar_border";
};

//! Optional configuration of a \ref main_window "main application window".

class main_window_config : public main_window_colors,
			   public menu_button_colors {


public:

	//! Specify optional position on the screen.

	void screen_position(const screen_positions &,
			     const std::string_view &);

	//! Loaded screen position.

	std::optional<std::tuple<std::string,
				 std::reference_wrapper<const screen_positions>>
		      > saved_position;

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
	static main_window create(const main_window_config_t &config,
				  functor &&f)
	{
		return do_create(config,
				 make_function<main_window_creator_t>
				 (std::forward<functor>(f)));
	}

	//! Create a new top level window using a custom layout manager.
	template<typename functor>
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
