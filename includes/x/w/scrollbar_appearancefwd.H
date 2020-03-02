/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_appearancefwd_h
#define x_w_scrollbar_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC scrollbar_appearanceObj;
class scrollbar_appearance_base;

/*! Appearance of the \ref scrollbar_config "scrollbar_config".

Specifies various properties of a scrollbar. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::scrollbar_config config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_scrollbar_appearance custom=INSERT_LIBX_NAMESPACE::w::scrollbar_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::scrollbar_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::scrollbar_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<scrollbar_appearanceObj,
	    scrollbar_appearance_base> scrollbar_appearance;

//! Constant \ref scrollbar_appearance "scrollbar appearance".

//! \see scrollbar_appearance

typedef const_ref<scrollbar_appearanceObj,
		  scrollbar_appearance_base> const_scrollbar_appearance;

//! A nullable pointer reference to a \ref scrollbar_appearance "scrollbar appearance".

//! \see scrollbar_appearance

typedef ptr<scrollbar_appearanceObj,
	    scrollbar_appearance_base> scrollbar_appearanceptr;

//! A nullable pointer reference to a const \ref scrollbar_appearance "scrollbar appearance".

//! \see scrollbar_appearance

typedef const_ptr<scrollbar_appearanceObj,
		  scrollbar_appearance_base> const_scrollbar_appearanceptr;

//! Alias

typedef appearance_wrapper<const_scrollbar_appearance> const_scrollbar_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
