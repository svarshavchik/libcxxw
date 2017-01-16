/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_elementfwd_h
#define x_w_elementfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class elementObj;

//! Methods common to all display elements.

//! \code
//! window->appdata=INSERT_LIBX_NAMESPACE::mcguffinstash<std::string>::create();
//!
//! INSERT_LIBX_NAMESPACE::w::screen screen=window->get_screen();
//!
//! window->show();
//!
//! window->hide();
//! \endcode
//!
//! \c appdata is an opaque object pointer for the application's use.
//! The library does not use it in any way.
//!
//! get_screen() returns the element's \ref screen "screen object".
//!
//! show() and hide() displays or hides the display element.

typedef ref<elementObj> element;

//! A nullable pointer reference to an element.

typedef ptr<elementObj> elementptr;


//! A constant generic window handler

//! \see element

typedef const_ref<elementObj> const_element;

LIBCXXW_NAMESPACE_END

#endif
