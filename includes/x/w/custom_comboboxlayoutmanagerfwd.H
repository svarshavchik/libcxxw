/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_comboboxlayoutmanagerfwd_h
#define x_w_custom_comboboxlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/w/connection_threadfwd.H>
#include <x/ptrfwd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN custom_combobox_selection_changed_info_t;
struct LIBCXX_HIDDEN custom_combobox_selection_search_info_t;
class LIBCXX_HIDDEN new_custom_comboboxlayoutmanager;

class LIBCXX_PUBLIC custom_comboboxlayoutmanagerObj;

/*! Custom combo-box layout managager.

The layout manager for a \ref new_custom_comboboxlayoutmanager "custom combo-box".

This is just a \ref listlayoutmanager "list layout manager" with
a current_selection() method that returns the display elemet for the currently
selected combo-box item.
*/

typedef ref<custom_comboboxlayoutmanagerObj> custom_comboboxlayoutmanager;

//! A constant \ref custom_comboboxlayoutmanager "combox-box layout manager".

//! \see custom_comboboxlayoutmanager

typedef const_ref<custom_comboboxlayoutmanagerObj> const_custom_comboboxlayoutmanager;

//! A nullable pointer reference to a \ref custom_comboboxlayoutmanager "combox-box layout manager".

//! \see custom_comboboxlayoutmanager

typedef ptr<custom_comboboxlayoutmanagerObj> custom_comboboxlayoutmanagerptr;

//! A nullable pointer reference to a const \ref custom_comboboxlayoutmanager "combox-box layout manager".

//! \see custom_comboboxlayoutmanager

typedef const_ptr<custom_comboboxlayoutmanagerObj> const_custom_comboboxlayoutmanagerptr;

//! Callback that gets invoked when a combo-box selection is made.

typedef functionref<void (THREAD_CALLBACK,
			  const custom_combobox_selection_changed_info_t &)
		      > custom_combobox_selection_changed_t;

LIBCXXW_NAMESPACE_END

#endif
