/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positionsfwd_h
#define x_w_screen_positionsfwd_h

#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class screen_positions;

#if 0
//! Save memorized coordinates.

//! \code
//! #include <INSERT_LIBX_NAMESPACE/config.H>
//!
//! INSERT_LIBX_NAMESPACE::w::screen_positions my_screen_positions;
//!
//! main_window->save(my_screen_positions);
//!
//! pos.screen_number.reset(); // Optional
//!
//!
//! INSERT_LIBX_NAMESPACE::w::save_screen_positions(INSERT_LIBX_NAMESPACE::configdir("app@example.com") + "/windows",
//!                             my_screen_positions);
//! \endcode
//!
//! Provides the means to save the current coordinates of
//! \ref main_window "main windows", so that they may be reopened in the same
//! position the next time the application runs.
//!
//! Use <a href="/configdir.html">LibCXX's configdir()</a> to prepare the
//! application's configuration directory. save_memorize_coordinates() first
//! parameter is the configuration file's name.
//!
//! \note
//! The window's screen number is also included, and can be reset() if
//! the window should be reopened on the default screen.
//!
//! \see load_screen_positions
//!
//! \see preserve_screen_number()
//!
//! \see generic_window

void save_screen_positions(const std::string_view &filename,
			   const screen_positions &coordinates);

//! Load memorized window locations.

//! \code
//! #include <INSERT_LIBX_NAMESPACE/config.H>
//!
//! INSERT_LIBX_NAMESPACE::w::screen_positions my_screen_positions=
//!     INSERT_LIBX_NAMESPACE::w::load_screen_positions(INSERT_LIBX_NAMESPACE::configdir("app@example.com") + "/windows");
//!
//! auto main_window=INSERT_LIBX_NAMESPACE::w::main_window::create
//!    (my_screen_positions,
//!     "main", // Unique identifier for this main window.
//!     []
//!     (const auto &main_window)
//!     {
//!          ...
//!     }
//! \endcode
//!
//! Provides the means to open the application's main window in the same
//! position as before.
//!
//! Use <a href="/configdir.html">LibCXX's configdir()</a> to prepare the
//! application's configuration directory. load_memorize_coordinates() first
//! parameter is the configuration file's name.
//!
//! No error occurs if the configuration file does not exist, in which case an
//! empty list of coordinates gets returned.
//!
//! \see save_screen_positions
//!
//! \see preserve_screen_number()
//!
//! \see generic_window

screen_positions load_screen_positions(const std::string_view &filename);

//! Whether saved screen positions preserve the application's screen number.

//! load_screen_positions() and save_screen_positions() include the
//! windows' screen numbers. Setting preserve_screen_number() to \c false
//! has the following results.
//!
//! - save_screen_positions() does not include the screen number in the
//! saved file.
//!
//! - load_screen_positions() does not restore the screen number in the
//! saved file.
//!
//! Creating a new \ref main_window "main window" and supplying a
//! saved position without a screen number results in the new window getting
//! created on the default screen. The window's position gets preserved if
//! the default screen resolution is the same as the window's original
//! screen.
#endif

void preserve_screen_number(bool flag);

LIBCXXW_NAMESPACE_END

#endif
