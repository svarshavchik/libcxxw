/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_font_picker_appearancefwd_h
#define x_w_font_picker_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class font_picker_appearanceObj;
class font_picker_appearance_base;

/*! Appearance of the \ref font_picker_config "font picker dialog".

Specifies various properties of a font picker. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::font_picker_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_font_picker_appearance custom=INSERT_LIBX_NAMESPACE::w::font_picker_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::font_picker_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::font_picker_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<font_picker_appearanceObj,
	    font_picker_appearance_base> font_picker_appearance;

//! Constant \ref font_picker_appearance "font picker appearance".

//! \see font_picker_appearance

typedef const_ref<font_picker_appearanceObj,
		  font_picker_appearance_base> const_font_picker_appearance;

//! A nullable pointer reference to a \ref font_picker_appearance "font picker appearance".

//! \see font_picker_appearance

typedef ptr<font_picker_appearanceObj,
	    font_picker_appearance_base> font_picker_appearanceptr;

//! A nullable pointer reference to a const \ref font_picker_appearance "font picker appearance".

//! \see font_picker_appearance

typedef const_ptr<font_picker_appearanceObj,
		  font_picker_appearance_base> const_font_picker_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
