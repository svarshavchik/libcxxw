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

//! Base class for a \ref popup_list_appearance "popup list appearance object".

class combobox_appearance_base : public ptrref_base {

public:

	static const const_combobox_appearance &theme();
};

LIBCXXW_NAMESPACE_END

#endif
