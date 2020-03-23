/*
** Copyright 2017-2020 Double Precision, Inc.
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

	//! If this is an editable combo-box return the current input.

	std::string editable_combobox_get() const;

	//! If this is an editable combo-box return the current input.

	std::u32string editable_combobox_get_unicode() const;
};

LIBCXXW_NAMESPACE_END

#endif
