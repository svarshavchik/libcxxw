/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_appearancefwd_h
#define x_w_list_appearancefwd_h

#include <x/w/appearancefwd.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC list_appearanceObj;
class list_appearance_base;

/*! List appearance

Specifies the appearance of a \ref listlayoutmanager "list", and lists used
in derivative widgets, like
\ref standard_comboboxlayoutmanager "combo-boxes".

When creating a new container that uses the list layout manager, or
derivatives from it, the \c appearance class member of
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager"
or its analogue is a constant reference to this appearance object.
This appearance object specifies various visual attributes of the list.
The \c appearance class member is a
\ref const_list_appearance "constant reference" and cannot be modified.

The
default appearance properties are specified by the current display theme,
and the default appearance object is cached for speed, and that's the
reason why it's constant.

\code
INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager nlm;

INSERT_LIBX_NAMESPACE::w::const_list_appearance custom=INSERT_LIBX_NAMESPACE::w::list_appearance::base::theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::list_appearance &custom)
             {
                    // Modify custom->...
             });

custom->border={};

nlm.appearance=custom;

\endcode

Use modify() to make a copy of an existing constant appearance object.
The passed-in closure receives a temporarily modifiable copy of the original
constant appearance object.

\c INSERT_LIBX_NAMESPACE::w::list_appearance::base::theme() returns the
default, cached, constant list appearance object.

\code
INSERT_LIBX_NAMESPACE::w::const_list_appearance custom=INSERT_LIBX_NAMESPACE::w::list_appearance::base::combobox_theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::list_appearance &custom)
             {
                    // Modify custom->...
             });

INSERT_LIBX_NAMESPACE::w::const_list_appearance custom=INSERT_LIBX_NAMESPACE::w::list_appearance::base::pane_theme()
    ->modify([]
             (const INSERT_LIBX_NAMESPACE::w::list_appearance &custom)
             {
                    // Modify custom->...
             });
\endcode

combobox_theme() and pane_theme() similarly return the cached default list
appearance themes for lists appearing in
\ref new_custom_comboboxlayoutmanager "combo-boxes" and
\ref panelayoutmanager "pane containers".

*/

typedef ref<list_appearanceObj, list_appearance_base> list_appearance;

//! Constant \ref list_appearance "list appearance properties".

//! \see list_appearance

typedef const_ref<list_appearanceObj,
		  list_appearance_base> const_list_appearance;

//! A nullable pointer reference to a \ref list_appearance "list appearance properties".

//! \see list_appearance

typedef ptr<list_appearanceObj,
	    list_appearance_base> list_appearanceptr;

//! A nullable pointer reference to a const \ref list_appearance "list appearance properties".

//! \see list_appearance

typedef const_ptr<list_appearanceObj,
		  list_appearance_base> const_list_appearanceptr;

//! Alias

typedef appearance_wrapper<const_list_appearance> const_list_appearance_wrapper;

LIBCXXW_NAMESPACE_END

#endif
