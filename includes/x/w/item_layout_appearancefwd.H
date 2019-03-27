/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_layout_appearancefwd_h
#define x_w_item_layout_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class item_layout_appearanceObj;
class item_layout_appearance_base;

/*! Appearance of the \ref new_itemlayoutmanager "item layout manager".

Specifies various properties of an item layout manager.
 The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::new_item_layoutmanager nlm{
    // ...
};

INSERT_LIBX_NAMESPACE::w::const_item_layout_appearance custom=INSERT_LIBX_NAMESPACE::w::item_layout_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::item_layout_appearance &custom)
             {
                    // Modify custom->...
             });

nlm.appearance=custom;
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::item_layout_appearance::base::theme() returns
a cached default theme appearance.

*/

typedef ref<item_layout_appearanceObj,
	    item_layout_appearance_base> item_layout_appearance;

//! Constant \ref item_layout_appearance "item layout manager appearance".

//! \see item_layout_appearance

typedef const_ref<item_layout_appearanceObj,
		  item_layout_appearance_base> const_item_layout_appearance;

//! A nullable pointer reference to a \ref item_layout_appearance "item layout manager appearance".

//! \see item_layout_appearance

typedef ptr<item_layout_appearanceObj,
	    item_layout_appearance_base> item_layout_appearanceptr;

//! A nullable pointer reference to a const \ref item_layout_appearance "item layout manager appearance".

//! \see item_layout_appearance

typedef const_ptr<item_layout_appearanceObj,
		  item_layout_appearance_base> const_item_layout_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
