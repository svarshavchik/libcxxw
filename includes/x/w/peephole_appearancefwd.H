/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peephole_appearancefwd_h
#define x_w_peephole_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC peephole_appearanceObj;
class peephole_appearance_base;

/*! Appearance of the \ref new_scrollable_peepholelayoutmanager "scrollable peephole".

Specifies various properties of a peephole. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::new_scrollable_peepholelayoutmanager lm{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_peephole_appearance custom=INSERT_LIBX_NAMESPACE::w::peephole_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::peephole_appearance &custom)
             {
                    // Modify custom->...
             });

lm.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::peephole_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<peephole_appearanceObj,
	    peephole_appearance_base> peephole_appearance;

//! Constant \ref peephole_appearance "peephole appearance".

//! \see peephole_appearance

typedef const_ref<peephole_appearanceObj,
		  peephole_appearance_base> const_peephole_appearance;

//! A nullable pointer reference to a \ref peephole_appearance "peephole appearance".

//! \see peephole_appearance

typedef ptr<peephole_appearanceObj,
	    peephole_appearance_base> peephole_appearanceptr;

//! A nullable pointer reference to a const \ref peephole_appearance "peephole appearance".

//! \see peephole_appearance

typedef const_ptr<peephole_appearanceObj,
		  peephole_appearance_base> const_peephole_appearanceptr;

//! Alias

typedef appearance_wrapper<const_peephole_appearance> const_peephole_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
