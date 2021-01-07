/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_file_dialog_appearancefwd_h
#define x_w_file_dialog_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC file_dialog_appearanceObj;
class file_dialog_appearance_base;

/*! Appearance of the \ref file_dialog_config "file dialog".

Specifies various properties of a file dialog. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::file_dialog_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_file_dialog_appearance custom=INSERT_LIBX_NAMESPACE::w::file_dialog_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::file_dialog_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::file_dialog_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<file_dialog_appearanceObj,
	    file_dialog_appearance_base> file_dialog_appearance;

//! Constant \ref file_dialog_appearance "file dialog appearance settings".

//! \see file_dialog_appearance

typedef const_ref<file_dialog_appearanceObj,
		  file_dialog_appearance_base> const_file_dialog_appearance;

//! A nullable pointer reference to a \ref file_dialog_appearance "file dialog appearance settings".

//! \see file_dialog_appearance

typedef ptr<file_dialog_appearanceObj,
	    file_dialog_appearance_base> file_dialog_appearanceptr;

//! A nullable pointer reference to a const \ref file_dialog_appearance "file dialog appearance settings".

//! \see file_dialog_appearance

typedef const_ptr<file_dialog_appearanceObj,
		  file_dialog_appearance_base> const_file_dialog_appearanceptr;

//! Alias

typedef appearance_wrapper<const_file_dialog_appearance> const_file_dialog_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
