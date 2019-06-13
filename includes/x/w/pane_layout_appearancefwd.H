/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_layout_appearancefwd_h
#define x_w_pane_layout_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC pane_layout_appearanceObj;
class pane_layout_appearance_base;

/*! Appearance of the \ref new_panelayoutmanager "pane layout manager".

Specifies various properties of a new pane layout. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::new_panelayoutmanager config{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_pane_layout_appearance custom=INSERT_LIBX_NAMESPACE::w::pane_layout_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::pane_layout_appearance &custom)
             {
                    // Modify custom->...
             });

config.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::pane_layout_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<pane_layout_appearanceObj,
	    pane_layout_appearance_base> pane_layout_appearance;

//! Constant \ref pane_layout_appearance "pane layout appearance".

//! \see pane_layout_appearance

typedef const_ref<pane_layout_appearanceObj,
		  pane_layout_appearance_base> const_pane_layout_appearance;

//! A nullable pointer reference to a \ref pane_layout_appearance "pane layout appearance".

//! \see pane_layout_appearance

typedef ptr<pane_layout_appearanceObj,
	    pane_layout_appearance_base> pane_layout_appearanceptr;

//! A nullable pointer reference to a const \ref pane_layout_appearance "pane layout appearance".

//! \see pane_layout_appearance

typedef const_ptr<pane_layout_appearanceObj,
		  pane_layout_appearance_base> const_pane_layout_appearanceptr;

//! Alias

typedef appearance_wrapper<const_pane_layout_appearance> const_pane_layout_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
