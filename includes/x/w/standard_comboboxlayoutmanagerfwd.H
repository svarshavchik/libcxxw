/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_standard_comboboxlayoutmanagerfwd_h
#define x_w_standard_comboboxlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class standard_comboboxlayoutmanagerObj;

/*! Standard combo-box layout managager.

A subclass of a \ref custom_comboboxlayoutmanager "custom combo-box layout manager"
that implements a standard combo-box containing plain text labels.

Only methods in this class should be used to modify the contents of the
combo-box. Do not use the methods from the inherited custom combo-box list
layout manager, with one exception:
a \ref list_lock "INSERT_LIBX_NAMESPACE::w::list_lock" may be instantiated
in order to lock access to the combo-box's items.

\code
lm->append_item("Item");
lm->insert_item(0, "Item");
lm->remove_item(0);
\encode

append_item() inserts a new item to the end of the combo-box's item list.
insert_item() inserts a new item before an existing item.
remove_item() removes an item.

\code
size_t n=lm->size();

INSERT_LIBX_NAMESPACE::w::text_param s=lm->item(0);
\encode

size() returns the number of items in the list, and item() returns the
given item number.

\code
std::optional<size_t> n=lm->selected();

if (n)
{
    size_t i=n.value();
}
\endcode

selected() returns the index of the currently-selected combo-box item.

*/

typedef ref<standard_comboboxlayoutmanagerObj> standard_comboboxlayoutmanager;

//! A constant \ref standard_comboboxlayoutmanager "standard combox-box layout manager".

//! \see standard_comboboxlayoutmanager

typedef const_ref<standard_comboboxlayoutmanagerObj> const_standard_comboboxlayoutmanager;

//! A nullable pointer reference to a \ref standard_comboboxlayoutmanager "standard combox-box layout manager".

//! \see standard_comboboxlayoutmanager

typedef ptr<standard_comboboxlayoutmanagerObj> standard_comboboxlayoutmanagerptr;

//! A nullable pointer reference to a const \ref standard_comboboxlayoutmanager "standard combox-box layout manager".

//! \see standard_comboboxlayoutmanager

typedef const_ptr<standard_comboboxlayoutmanagerObj> const_standard_comboboxlayoutmanagerptr;

class new_standard_comboboxlayoutmanager;


LIBCXXW_NAMESPACE_END

#endif
