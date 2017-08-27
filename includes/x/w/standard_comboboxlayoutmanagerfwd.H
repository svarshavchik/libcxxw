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

Use only the methods documented here to modify the contents of the
combo-box. Do not use the methods from the inherited custom combo-box list
layout manager, and the underlying list layout manager, except as noted.

\code
lm->append_item("Item");
lm->insert_item(0, "Item");
lm->remove_item(0);
lm->replace_all({"Lorem","Ipsum"});
\endcode

append_item() inserts a new item to the end of the combo-box's item list.
insert_item() inserts a new item before an existing item.
remove_item() removes an item.

replace_all()'s parameter is a vector of text items, which replaces all
existing items in the combo-box.

\code
size_t n=lm->size();

INSERT_LIBX_NAMESPACE::w::text_param s=lm->item(0);
\endcode

size() returns the number of items in the list, and item() returns the
given item number.

\code
l->append_separator();

l->insert_separator(0);

l->replace_separator(0);
\endcode

These methods, inherited from the underlying
\ref listlayoutmanager "list layout manager", add a separator item.

\code
lm->autoselect(1);

std::optional<size_t> n=lm->selected();

if (n)
{
    size_t i=n.value();
    lm->unselect();
}
\endcode

selected() returns the index of the currently-selected combo-box item.

autoselect() and unselect() are inherited. autoselect() manually selects
an item in the list, and unselect() unselects the existing combo-box
selection.

\code
INSERT_LIBX_NAMESPACE::w::standard_combobox_lock lock{lm};
\endcode

A \ref list_lock "INSERT_LIBX_NAMESPACE::w::standard_combobox_lock" may be instantiated
in order to lock access to the combo-box's items.

\par Enabling and disabling list items.

Standard combo-boxes inherit the \c enabled() method from the underlying
list layout manager, in order to enable or disabling individual list items.

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

class standard_combobox_lock;

LIBCXXW_NAMESPACE_END

#endif
