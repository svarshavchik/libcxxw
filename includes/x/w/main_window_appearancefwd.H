/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_window_appearancefwd_h
#define x_w_main_window_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC main_window_appearanceObj;
class main_window_appearance_base;

/*! Appearance of the \ref main_window_config "main_window_config".

Specifies various properties of a main window. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::main_window_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_main_window_appearance custom=INSERT_LIBX_NAMESPACE::w::main_window_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::main_window_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::main_window_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<main_window_appearanceObj,
	    main_window_appearance_base> main_window_appearance;

//! Constant \ref main_window_appearance "main window appearance".

//! \see main_window_appearance

typedef const_ref<main_window_appearanceObj,
		  main_window_appearance_base> const_main_window_appearance;

//! A nullable pointer reference to a \ref main_window_appearance "main window appearance".

//! \see main_window_appearance

typedef ptr<main_window_appearanceObj,
	    main_window_appearance_base> main_window_appearanceptr;

//! A nullable pointer reference to a const \ref main_window_appearance "main window appearance".

//! \see main_window_appearance

typedef const_ptr<main_window_appearanceObj,
		  main_window_appearance_base> const_main_window_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
