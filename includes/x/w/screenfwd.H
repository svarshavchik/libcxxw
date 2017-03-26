/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screenfwd_h
#define x_w_screenfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class screenObj;
class screenBase;

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
          INSERT_LIBX_NAMESPACE::w::new_layoutmanager::base::create_grid());
\endcode

create_mainwindow() creates a new \ref main_window "main window".
The first parameter is a creator lambda, the second parameter is optional
and specifies the new main window's layout manager.

A \ref main_window "main window" may also be created directly:

\code
#include <INSERT_LIBX_NAMESPACE/w/main_window.H>

auto main_window=INSERT_LIBX_NAMESPACE::w::main_window::create(
          []
          (const INSERT_LIBX_NAMESPACE::w::main_window &main_window)
          {
              /// ...
          },
          INSERT_LIBX_NAMESPACE::w::new_layoutmanager::base::create_grid());
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

INSERT_LIBX_NAMESPACE::w::dim_t left, right, top, bottom;

bool flag=s->get_frame_extents(left, right, top, bottom);
\endcode

\c get_workarea() returns the screen's work area, get_frame_extents() returns
the size of the window manager's borders for a top level window, in pixels,
returning \c true or \c false indicating whether the window manager provided
this information.

*/

typedef ref<screenObj, screenBase> screen;

//! A constant screen

typedef const_ref<screenObj, screenBase> const_screen;

LIBCXXW_NAMESPACE_END

#endif
