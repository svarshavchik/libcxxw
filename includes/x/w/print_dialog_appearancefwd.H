/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_print_dialog_appearancefwd_h
#define x_w_print_dialog_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC print_dialog_appearanceObj;
class print_dialog_appearance_base;

/*! Appearance of the \ref print_dialog_config "print_dialog dialog".

Specifies various properties of a print_dialog. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::print_dialog_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_print_dialog_appearance custom=INSERT_LIBX_NAMESPACE::w::print_dialog_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::print_dialog_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::print_dialog_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<print_dialog_appearanceObj,
	    print_dialog_appearance_base> print_dialog_appearance;

//! Constant \ref print_dialog_appearance "print dialog appearance".

//! \see print_dialog_appearance

typedef const_ref<print_dialog_appearanceObj,
		  print_dialog_appearance_base> const_print_dialog_appearance;

//! A nullable pointer reference to a \ref print_dialog_appearance "print dialog appearance".

//! \see print_dialog_appearance

typedef ptr<print_dialog_appearanceObj,
	    print_dialog_appearance_base> print_dialog_appearanceptr;

//! A nullable pointer reference to a const \ref print_dialog_appearance "print dialog appearance".

//! \see print_dialog_appearance

typedef const_ptr<print_dialog_appearanceObj,
		  print_dialog_appearance_base> const_print_dialog_appearanceptr;

//! Alias

typedef appearance_wrapper<const_print_dialog_appearance> const_print_dialog_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
