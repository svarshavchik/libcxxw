/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editable_comboboxlayoutmanagerfwd_h
#define x_w_editable_comboboxlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class editable_comboboxlayoutmanagerObj;

/*! Editable combo-box layout managager.

A subclass of a \ref standard_comboboxlayoutmanager "standard combo-box layout manager"
that uses an editable \ref input_field "input_field" instead of a read-only
label for the currently selected combo-box item, to provide an alternative
of typing in the currently-selected value in addition to selecting one of
the pre-set ones.

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

LIBCXXW_NAMESPACE_END

#endif
