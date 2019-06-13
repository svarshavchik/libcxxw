/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pane_appearancefwd_h
#define x_w_pane_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC pane_appearanceObj;
class pane_appearance_base;

/*! Appearance of the next \ref panefactory "new pane".

Specifies various properties of a new pane. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::panefactory f;

INSERT_LIBX_NAMESPACE::w::const_pane_appearance custom=INSERT_LIBX_NAMESPACE::w::pane_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::pane_appearance &custom)
             {
                    // Modify custom->...
             });

f->appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::pane_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<pane_appearanceObj,
	    pane_appearance_base> pane_appearance;

//! Constant \ref pane_appearance "pane appearance".

//! \see pane_appearance

typedef const_ref<pane_appearanceObj,
		  pane_appearance_base> const_pane_appearance;

//! A nullable pointer reference to a \ref pane_appearance "pane appearance".

//! \see pane_appearance

typedef ptr<pane_appearanceObj,
	    pane_appearance_base> pane_appearanceptr;

//! A nullable pointer reference to a const \ref pane_appearance "pane appearance".

//! \see pane_appearance

typedef const_ptr<pane_appearanceObj,
		  pane_appearance_base> const_pane_appearanceptr;

//! Alias

typedef appearance_wrapper<const_pane_appearance> const_pane_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
