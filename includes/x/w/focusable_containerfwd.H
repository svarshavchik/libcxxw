/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_containerfwd_h
#define x_w_focusable_containerfwd_h

#include <x/w/containerfwd.H>
#include <x/w/focusablefwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC focusable_containerObj;

//! A focusable widget that's implemented as an internal container.

//! Base class for widgets that multiply-inherit from a
//! \ref container "container" and a \ref focusable "focusable".
//!
//! Use the \ref factory "factory"'s create_focusable_container() to
//! create these widgets, instead of create_container().
//!
//! \par Combo-box helpers
//!
//! The following helper functions require the focusable container with
//! the appropriate layout manager, otherwise an exception gets thrown.
//!
//! \code
//! auto e=container->combobox_current_selection();
//! \endcode
//!
//! This is equivalent to using get_layoutmanager() to obtain the combo-box
//! layout manager, then using current_selection() to retrieve the combo-box's
//! \ref element "element widget" that represents the combo-box's current value.
//!
//! \code
//! std::string s=container->editable_combobox_get();
//!
//! std::u32string us=container->editable_combobox_get_unicode();
//! \endcode
//!
//! This is equivalent to using get_layoutmanager() to obtain the
//! \ref editable_comboboxlayoutmanager "editable combo-box layout manager"
//! and then using its get() methods to retrieve that contents of the
//! editable combobox's internal input field.

typedef ref<focusable_containerObj> focusable_container;

//! A constant focusable container.

//! \see focusable_container

typedef const_ref<focusable_containerObj> const_focusable_container;

//! A nullable pointer reference to a focusable container.

//! \see focusable_container

typedef ptr<focusable_containerObj> focusable_containerptr;

//! A nullable pointer reference to a constant focusable container.

//! \see focusable_container

typedef const_ptr<focusable_containerObj> const_focusable_containerptr;

LIBCXXW_NAMESPACE_END

#endif
