/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_color_picker_appearancefwd_h
#define x_w_color_picker_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC color_picker_appearanceObj;
class color_picker_appearance_base;

/*! Appearance of the \ref color_picker_config "color_picker".

Specifies various properties of a color picker. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::color_picker_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_color_picker_appearance custom=INSERT_LIBX_NAMESPACE::w::color_picker_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::color_picker_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::color_picker_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<color_picker_appearanceObj,
	    color_picker_appearance_base> color_picker_appearance;

//! Constant \ref color_picker_appearance "color picker appearance".

//! \see color_picker_appearance

typedef const_ref<color_picker_appearanceObj,
		  color_picker_appearance_base> const_color_picker_appearance;

//! A nullable pointer reference to a \ref color_picker_appearance "color picker appearance".

//! \see color_picker_appearance

typedef ptr<color_picker_appearanceObj,
	    color_picker_appearance_base> color_picker_appearanceptr;

//! A nullable pointer reference to a const \ref color_picker_appearance "color picker appearance".

//! \see color_picker_appearance

typedef const_ptr<color_picker_appearanceObj,
		  color_picker_appearance_base> const_color_picker_appearanceptr;

//! Alias

typedef appearance_wrapper<const_color_picker_appearance> const_color_picker_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
