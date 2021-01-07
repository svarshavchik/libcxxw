/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_appearancefwd_h
#define x_w_date_input_field_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC date_input_field_appearanceObj;
class date_input_field_appearance_base;

/*! Appearance of the \ref date_input_field_config "date_input_field".

Specifies various properties of a date_input_field. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::date_input_field_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_date_input_field_appearance custom=INSERT_LIBX_NAMESPACE::w::date_input_field_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::date_input_field_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::date_input_field_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<date_input_field_appearanceObj,
	    date_input_field_appearance_base> date_input_field_appearance;

//! Constant \ref date_input_field_appearance "date input field appearance".

//! \see date_input_field_appearance

typedef const_ref<date_input_field_appearanceObj,
		  date_input_field_appearance_base> const_date_input_field_appearance;

//! A nullable pointer reference to a \ref date_input_field_appearance "date input field appearance".

//! \see date_input_field_appearance

typedef ptr<date_input_field_appearanceObj,
	    date_input_field_appearance_base> date_input_field_appearanceptr;

//! A nullable pointer reference to a const \ref date_input_field_appearance "date input field appearance".

//! \see date_input_field_appearance

typedef const_ptr<date_input_field_appearanceObj,
		  date_input_field_appearance_base> const_date_input_field_appearanceptr;

//! Alias

typedef appearance_wrapper<const_date_input_field_appearance> const_date_input_field_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
