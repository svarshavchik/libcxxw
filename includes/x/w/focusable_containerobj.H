/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_containerobj_h
#define x_w_focusable_containerobj_h

#include <x/w/focusable_containerfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A focusable widget that's implemented as an internal container.

//! \see focusable_container

class focusable_containerObj : public containerObj, public focusableObj {

public:
	using containerObj::containerObj;

	//! Destructor

	~focusable_containerObj();

	//! If this is a combo-box return its current selection.

	element combobox_current_selection();

	//! If this is a combo-box return its current selection.

	const_element combobox_current_selection() const;

	//! If this is an editable combo-box: return the input field.

	input_field editable_combobox_input_field();

	//! If this is an editable combo-box: return the input field.

	const_input_field editable_combobox_input_field() const;
};

LIBCXXW_NAMESPACE_END

#endif
