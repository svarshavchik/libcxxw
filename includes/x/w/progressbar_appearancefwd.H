/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbar_appearancefwd_h
#define x_w_progressbar_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC progressbar_appearanceObj;
class progressbar_appearance_base;

/*! Appearance of a \ref progressbar_config "progress bar".

Specifies various properties of a progress bar. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::progressbar_config config;

INSERT_LIBX_NAMESPACE::w::const_progressbar_appearance custom=INSERT_LIBX_NAMESPACE::w::progressbar_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::progressbar_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;

INSERT_LIBX_NAMESPACE::w::progressbar
    pb=factory->create_progressbar(
         []
         (const auto &)
         {
               // ...
         },
         config);
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::progressbar_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<progressbar_appearanceObj,
	    progressbar_appearance_base> progressbar_appearance;

//! Constant \ref progressbar_appearance "progressbar appearance".

//! \see progressbar_appearance

typedef const_ref<progressbar_appearanceObj,
		  progressbar_appearance_base> const_progressbar_appearance;

//! A nullable pointer reference to a \ref progressbar_appearance "progressbar appearance".

//! \see progressbar_appearance

typedef ptr<progressbar_appearanceObj,
	    progressbar_appearance_base> progressbar_appearanceptr;

//! A nullable pointer reference to a const \ref progressbar_appearance "progressbar appearance".

//! \see progressbar_appearance

typedef const_ptr<progressbar_appearanceObj,
		  progressbar_appearance_base> const_progressbar_appearanceptr;

//! Alias

typedef appearance_wrapper<const_progressbar_appearance> const_progressbar_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
