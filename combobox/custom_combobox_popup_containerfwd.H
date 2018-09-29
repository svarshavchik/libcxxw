/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_combobox_popup_containerfwd_h
#define x_w_custom_combobox_popup_containerfwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN custom_combobox_popup_containerObj;

/*! The combo-box popup container.

The container with a list layout manager, comprising the list of items
in the combo-box's popup.

*/

typedef ref<custom_combobox_popup_containerObj> custom_combobox_popup_container;

//! A constant \ref custom_combobox_popup_container "combo-box popup container".

//! \see custom_combobox_popup_container

typedef const_ref<custom_combobox_popup_containerObj> const_custom_combobox_popup_container;

//! A nullable pointer reference to a \ref custom_combobox_popup_container "combo-box popup container".

//! \see custom_combobox_popup_container

typedef ptr<custom_combobox_popup_containerObj> custom_combobox_popup_containerptr;

//! A nullable pointer reference to a const \ref custom_combobox_popup_container "combo-box popup container".

//! \see custom_combobox_popup_container

typedef const_ptr<custom_combobox_popup_containerObj> const_custom_combobox_popup_containerptr;

LIBCXXW_NAMESPACE_END

#endif
