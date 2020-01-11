/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editable_comboboxlayoutmanagerfwd_h
#define x_w_editable_comboboxlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN editable_combobox_selection_changed_info_t;
class LIBCXX_HIDDEN new_editable_comboboxlayoutmanager;
class LIBCXX_PUBLIC editable_comboboxlayoutmanagerObj;

/*! Editable combo-box layout managager.

A subclass of a \ref standard_comboboxlayoutmanager "standard combo-box layout manager"
that uses an editable \ref input_field "input_field" instead of a read-only
label for the currently selected combo-box item.

Typing in the combo-box's input field auto-completes the closest matching
list item, if possible; but it's possible to type in something that does
not match any combo-box list item.

\ref new_editable_comboboxlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_editable_comboboxlayoutmanager"
creates a
\ref focusable_container "focusable container" that uses the editable
combo-box layout manager.

*/

typedef ref<editable_comboboxlayoutmanagerObj> editable_comboboxlayoutmanager;

//! A constant \ref editable_comboboxlayoutmanager "editable combo-box layout manager".

//! \see editable_comboboxlayoutmanager

typedef const_ref<editable_comboboxlayoutmanagerObj> const_editable_comboboxlayoutmanager;

//! A nullable pointer reference to a \ref editable_comboboxlayoutmanager "editable combo-box layout manager".

//! \see editable_comboboxlayoutmanager

typedef ptr<editable_comboboxlayoutmanagerObj> editable_comboboxlayoutmanagerptr;

//! A nullable pointer reference to a const \ref editable_comboboxlayoutmanager "editable combo-box layout manager".

//! \see editable_comboboxlayoutmanager

typedef const_ptr<editable_comboboxlayoutmanagerObj> const_editable_comboboxlayoutmanagerptr;

struct editable_combobox_selection_changed_info_t;

/*!

Callback that gets invoked when an editable combo-box's selection changes.

*/

typedef functionref<void (THREAD_CALLBACK,
			  const editable_combobox_selection_changed_info_t &)
		      > editable_combobox_selection_changed_t;

LIBCXXW_NAMESPACE_END

#endif
