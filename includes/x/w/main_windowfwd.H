/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_windowfwd_h
#define x_w_main_windowfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class main_windowObj;
class main_windowBase;

//! Top level application window.
//!
//! \code
//!
//! auto screen=INSERT_LIBX_NAMESPACE::w::screen::create();
//!
//! INSERT_LIBX_NAMESPACE::w::mainwindow=screen->create_mainwindow(
//!                 []
//!                 (const INSERT_LIBX_NAMESPACE::w::mainwindow &w)
//!                 {
//!                        // Fill in the contents of the new window.
//!                 });
//!
//! \endcode

typedef ref<main_windowObj, main_windowBase> main_window;

//! A reference to a constant main application window.

//! \see main_window

typedef const_ref<main_windowObj, main_windowBase> const_main_window;

//! A nullable pointer reference to a main application window.

//! \see main_window

typedef ptr<main_windowObj, main_windowBase> main_windowptr;

//! A nullable pointer reference to a constant main application window.

//! \see main_window

typedef const_ptr<main_windowObj, main_windowBase> const_main_windowptr;

LIBCXXW_NAMESPACE_END

#endif
