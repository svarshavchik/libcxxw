/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screenfwd_h
#define x_w_screenfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC screenObj;
class LIBCXX_PUBLIC screenBase;

/*! A screen on a display server.

\code

auto s=INSERT_LIBX_NAMESPACE::w::screen::create();

auto s=INSERT_LIBX_NAMESPACE::w::screen::create(1);

auto s=INSERT_LIBX_NAMESPACE::w::screen::create(INSERT_LIBX_NAMESPACE::w::connection::create("localhost:0.0"));

auto s=INSERT_LIBX_NAMESPACE::w::screen::create(INSERT_LIBX_NAMESPACE::w::connection::create("localhost:0.0"), 1);

INSERT_LIBX_NAMESPACE::w::connection conn=s->get_connection()

\endcode

create() returns a screen object for the default display of the default
display server connection, a specific screen object, or a screen object
for the explicitly specified connection, depending on the parameters.

get_connection() returns the existing screen object's
\ref connection "connection object".

\code
#include <INSERT_LIBX_NAMESPACE/w/main_window.H>

INSERT_LIBX_NAMESPACE::w::main_window main_window=s->create_mainwindow(
          []
          (const INSERT_LIBX_NAMESPACE::w::main_window &main_window)
          {
              /// ...
          },
          INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{});
\endcode

create_mainwindow() creates a new \ref main_window "main window".
The first parameter is a creator lambda, the second parameter is optional
and specifies the new main window's layout manager.

\code
INSERT_LIBX_NAMESPACE::w::screen_positions_t screen_positions=
    INSERT_LIBX_NAMESPACE::w::load_screen_positions(INSERT_LIBX_NAMESPACE::configdir("app@example.com") + "/windows");

INSERT_LIBX_NAMESPACE::w::main_window_config config;

config.screen_position(screen_positions, "main");

INSERT_LIBX_NAMESPACE::w::main_window main_window=s->create(config,
     []
     (const INSERT_LIBX_NAMESPACE::w::main_window &new_main_window)
     {
          // Initialize the new main window.
     });
\endcode

The main window's former's position can be saved, loaded with
load_screen_positions(), then passed in order to reopen the main window
in the same location.

This is done by constructing a \ref main_window_config "main_window_config"
and using its screen_position() method.

\note
If the saved position includes a screen number, the window will really be
opened on its former screen, which may not necessarily be this screen.

A \ref main_window "main window" may also be created directly:

\code
#include <INSERT_LIBX_NAMESPACE/w/main_window.H>

auto main_window=INSERT_LIBX_NAMESPACE::w::main_window::create(
          []
          (const INSERT_LIBX_NAMESPACE::w::main_window &main_window)
          {
              /// ...
          },
          INSERT_LIBX_NAMESPACE::w::new_gridlayoutmanager{});
\endcode

This creates a new main window on the default screen of the default
X server.

\code
INSERT_LIBX_NAMESPACE::w::dim_t width=s->width_in_pixels();
INSERT_LIBX_NAMESPACE::w::dim_t height=s->height_in_pixels();
INSERT_LIBX_NAMESPACE::w::dim_t widthmm=s->width_in_millimeters();
INSERT_LIBX_NAMESPACE::w::dim_t heightmm=s->height_in_millimeters();
\endcode

Determine the size of the screen.

\code
INSERT_LIBX_NAMESPACE::w::rectangle size=s->get_workarea();
\endcode

\c get_workarea() returns the screen's work area, the usable area of the
screen.

\par Selections

\code
bool flag=conn->selection_has_owner("SECONDARY");
\endcode

For convenience, this is forwarded to this screen's
\ref connection "connection".
*/

typedef ref<screenObj, screenBase> screen;

//! A constant screen

typedef const_ref<screenObj, screenBase> const_screen;

LIBCXXW_NAMESPACE_END

#endif
