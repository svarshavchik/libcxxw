/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tooltip_border_appearancefwd_h
#define x_w_tooltip_border_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC tooltip_border_appearanceObj;
class tooltip_border_appearance_base;

/*! Appearance of the \ref tooltip_appearance "tooltip" border.

Specifies various properties of a tooltip border. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::const_tooltip_border_appearance custom=INSERT_LIBX_NAMESPACE::w::tooltip_border_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::tooltip_border_appearance &custom)
             {
                    // Modify custom->...
             });
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::tooltip_border_appearance::base::theme() returns
a cached default theme appearance.
\c INSERT_LIBX_NAMESPACE::w::tooltip_border_appearance::base::nonalpha_theme()
returns a cached default theme appearance for displays that don't have an
alpha channel.

*/

typedef ref<tooltip_border_appearanceObj,
	    tooltip_border_appearance_base> tooltip_border_appearance;

//! Constant \ref tooltip_border_appearance "WHATITIS".

//! \see tooltip_border_appearance

typedef const_ref<tooltip_border_appearanceObj,
		  tooltip_border_appearance_base> const_tooltip_border_appearance;

//! A nullable pointer reference to a \ref tooltip_border_appearance "WHATITIS".

//! \see tooltip_border_appearance

typedef ptr<tooltip_border_appearanceObj,
	    tooltip_border_appearance_base> tooltip_border_appearanceptr;

//! A nullable pointer reference to a const \ref tooltip_border_appearance "WHATITIS".

//! \see tooltip_border_appearance

typedef const_ptr<tooltip_border_appearanceObj,
		  tooltip_border_appearance_base> const_tooltip_border_appearanceptr;

//! Alias

typedef appearance_wrapper<const_tooltip_border_appearance> const_tooltip_border_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
