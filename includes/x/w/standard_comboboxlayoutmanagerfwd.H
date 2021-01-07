/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_standard_comboboxlayoutmanagerfwd_h
#define x_w_standard_comboboxlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN standard_combobox_selection_changed_info_t;
class LIBCXX_HIDDEN new_standard_comboboxlayoutmanager;
class LIBCXX_PUBLIC standard_comboboxlayoutmanagerObj;
class LIBCXX_PUBLIC const_standard_combobox_lock;
class LIBCXX_PUBLIC standard_combobox_lock;

/*! Standard combo-box layout managager.

A subclass of a \ref custom_comboboxlayoutmanager "custom combo-box layout manager"
that implements a standard combo-box containing plain text labels.

\ref new_standard_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_standard_comboboxlayoutmanager"
creates a
\ref focusable_container "focusable container" that uses the standard
combo-box layout manager.

Use only the methods documented here to modify the contents of the
combo-box. Do not use the methods from the inherited custom combo-box list
layout manager, and the underlying list layout manager, except as noted.

\code
lm->append_items("Item");
lm->insert_items(0, "Item");
lm->remove_item(0);
lm->replace_all({"Lorem","Ipsum"});

std::vector<size_t> index;

lm->resort_items(index);
\endcode

append_items() inserts a new item to the end of the combo-box's item list.
insert_items() inserts a new item before an existing item.
remove_item() removes an item.

replace_all()'s parameter is a vector of text items, which replaces all
existing items in the combo-box.

These methods are inherited from the underlying
\ref listlayoutmanager "list layout manager",
and the parameters are actually
\ref list_item_param "list_item_param" variants, however the only permissible
items in a standard combo-box item are text labels,
\c INSERT_LIBX_NAMESPACE::w::separator, a
list item status change callback, or using
an \c x::w::new_items to obtain a \ref listitemhandle "handle" to the
new list items.

\code
size_t n=lm->size();

INSERT_LIBX_NAMESPACE::w::text_param s=lm->item(0);
\endcode

size() returns the number of items in the list, and item() returns the
given item number.

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

\par Internal thread methods

Standard combo-boxes inherit the \c IN_THREAD and the non\c IN_THREAD
versions of the methods from the underlying
\ref listlayoutmanager "list layout manager", which
work in the analogous way.

\par Generating contents from an XML-based specification

\code
lm->generate("standard-combobox-layout", generators, elements);
\endcode

generate() generates the contents of the combo-box from the
loaded theme file. See the tutorial for more information.

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

/*!

Callback that gets invoked when a standard combo-box's selection changes.

*/

typedef functionref<void (THREAD_CALLBACK,
			  const standard_combobox_selection_changed_info_t &)
		      > standard_combobox_selection_changed_t;

LIBCXXW_NAMESPACE_END

#endif
