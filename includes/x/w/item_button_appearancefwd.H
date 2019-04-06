/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_button_appearancefwd_h
#define x_w_item_button_appearancefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class item_button_appearanceObj;
class item_button_appearance_base;

/*! Appearance of the buttons created by the \ref new_itemlayoutmanager "item layout manager".

Specifies various properties of a item layout manager's button. The default properties are
set by the current display theme.

\code
INSERT_LIBX_NAMESPACE::w::itemlayoutmanager lm;

INSERT_LIBX_NAMESPACE::w::const_item_button_appearance custom=INSERT_LIBX_NAMESPACE::w::item_button_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::item_button_appearance &custom)
             {
                    // Modify custom->...
             });

// ...

lm->append_item([]
                (const INSERT_LIBX_NAMESPACE::w::factory &f)
                {
                     // ...
                },
                custom);
\endcode

The default properties are set by the current display theme, and are
cached for speed.
\c INSERT_LIBX_NAMESPACE::w::item_button_appearance::base::theme() returns
a cached default theme appearance.

Item layout manager's append_item() and insert_item() take an optional
parameter that specifies custom properties for the new button.
*/

typedef ref<item_button_appearanceObj,
	    item_button_appearance_base> item_button_appearance;

//! Constant \ref item_button_appearance "item button appearance".

//! \see item_button_appearance

typedef const_ref<item_button_appearanceObj,
		  item_button_appearance_base> const_item_button_appearance;

//! A nullable pointer reference to a \ref item_button_appearance "item button appearance".

//! \see item_button_appearance

typedef ptr<item_button_appearanceObj,
	    item_button_appearance_base> item_button_appearanceptr;

//! A nullable pointer reference to a const \ref item_button_appearance "item button appearance".

//! \see item_button_appearance

typedef const_ptr<item_button_appearanceObj,
		  item_button_appearance_base> const_item_button_appearanceptr;

LIBCXXW_NAMESPACE_END

#endif
