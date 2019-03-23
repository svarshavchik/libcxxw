/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_image_button_appearancefwd_h
#define x_w_image_button_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class image_button_appearanceObj;
class image_button_appearance_base;

/*! Appearance of the \ref image_button "image_buttons".

Specifies various properties of image_buttons. The default properties are
set by the current display theme.

\code

INSERT_LIBX_NAMESPACE::w::image_button_appearance custom=INSERT_LIBX_NAMESPACE::w::image_button_appearance::base::checkbox_theme()->clone();

factory->create_checkbox(custom);
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::image_button_appearance::base::checkbox_theme()
 returns a cached default theme appearance for checkboxes.
\c INSERT_LIBX_NAMESPACE::w::image_button_appearance::base::radio_theme()
 returns a cached default theme appearance for radio buttons.


*/

typedef ref<image_button_appearanceObj,
	    image_button_appearance_base> image_button_appearance;

//! Constant \ref image_button_appearance "image button appearance".

//! \see image_button_appearance

typedef const_ref<image_button_appearanceObj,
		  image_button_appearance_base> const_image_button_appearance;

//! A nullable pointer reference to a \ref image_button_appearance "image button appearance".

//! \see image_button_appearance

typedef ptr<image_button_appearanceObj,
	    image_button_appearance_base> image_button_appearanceptr;

//! A nullable pointer reference to a const \ref image_button_appearance "image button appearance".

//! \see image_button_appearance

typedef const_ptr<image_button_appearanceObj,
		  image_button_appearance_base
		  > const_image_button_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
