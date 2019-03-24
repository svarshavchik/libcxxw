/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_combobox_appearance_h
#define x_w_combobox_appearance_h

#include <x/w/combobox_appearancefwd.H>
#include <x/w/combobox_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref combobox_appearance "combobox appearance object".

//! Refer to this clas as combobox_appearance::base

class combobox_appearance_base : public ptrref_base {

public:

	//! Default combo-box theme.
	static const const_combobox_appearance &theme();
};
LIBCXXW_NAMESPACE_END

#endif
