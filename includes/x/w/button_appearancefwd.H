/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_button_appearancefwd_h
#define x_w_button_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC button_appearanceObj;
class button_appearance_base;

/*! Appearance of the \ref button_config "button_config".

Specifies various properties of a button. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::button_config config;

INSERT_LIBX_NAMESPACE::w::const_button_appearance custom=INSERT_LIBX_NAMESPACE::w::button_appearance::base::normal_theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::button_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::button_appearance::base::normal_theme()
and
\c INSERT_LIBX_NAMESPACE::w::button_appearance::base::default_theme()
return a cached default theme appearance.

*/

typedef ref<button_appearanceObj,
	    button_appearance_base> button_appearance;

//! Constant \ref button_appearance "button appearance".

//! \see button_appearance

typedef const_ref<button_appearanceObj,
		  button_appearance_base> const_button_appearance;

//! A nullable pointer reference to a \ref button_appearance "button appearance".

//! \see button_appearance

typedef ptr<button_appearanceObj,
	    button_appearance_base> button_appearanceptr;

//! A nullable pointer reference to a const \ref button_appearance "button appearance".

//! \see button_appearance

typedef const_ptr<button_appearanceObj,
		  button_appearance_base> const_button_appearanceptr;

//! Alias

typedef appearance_wrapper<const_button_appearance> const_button_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
