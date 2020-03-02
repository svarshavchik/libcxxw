/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_appearancefwd_h
#define x_w_input_field_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC input_field_appearanceObj;
class input_field_appearance_base;

/*! Appearance of the \ref input_field "input_field".

Specifies various properties of a input_field. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::input_field_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_input_field_appearance custom=INSERT_LIBX_NAMESPACE::w::input_field_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::input_field_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::input_field_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<input_field_appearanceObj,
	    input_field_appearance_base> input_field_appearance;

//! Constant \ref input_field_appearance "input field appearance".

//! \see input_field_appearance

typedef const_ref<input_field_appearanceObj,
		  input_field_appearance_base> const_input_field_appearance;

//! A nullable pointer reference to a \ref input_field_appearance "input field appearance".

//! \see input_field_appearance

typedef ptr<input_field_appearanceObj,
	    input_field_appearance_base> input_field_appearanceptr;

//! A nullable pointer reference to a const \ref input_field_appearance "input field appearance".

//! \see input_field_appearance

typedef const_ptr<input_field_appearanceObj,
		  input_field_appearance_base> const_input_field_appearanceptr;

//! Alias

typedef appearance_wrapper<const_input_field_appearance> const_input_field_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
