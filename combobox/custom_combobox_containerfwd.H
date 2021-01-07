/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_combobox_containerfwd_h
#define x_w_custom_combobox_containerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN custom_combobox_containerObj;

/*! A custom combo-box.

A focusable container with a
\ref custom_comboboxlayoutmanager "custom combo-box layout manager".

The container contains:

- A button that shows/hides the combo-box's popup.

*/

typedef ref<custom_combobox_containerObj> custom_combobox_container;

//! A constant \ref custom_combobox_container "custom combo-box".

//! \see custom_combobox_container

typedef const_ref<custom_combobox_containerObj> const_custom_combobox_container;

//! A nullable pointer reference to a \ref custom_combobox_container "custom combo-box".

//! \see custom_combobox_container

typedef ptr<custom_combobox_containerObj> custom_combobox_containerptr;

//! A nullable pointer reference to a const \ref custom_combobox_container "custom combo-box".

//! \see custom_combobox_container

typedef const_ptr<custom_combobox_containerObj> const_custom_combobox_containerptr;

LIBCXXW_NAMESPACE_END

#endif
