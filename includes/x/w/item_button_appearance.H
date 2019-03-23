/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_button_appearance_h
#define x_w_item_button_appearance_h

#include <x/w/item_button_appearancefwd.H>
#include <x/w/item_button_appearanceobj.H>
#include <x/w/image_button_appearance.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref item_button_appearance "item_button appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::item_button_appearance::base.

class item_button_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const const_item_button_appearance &theme();

};

LIBCXXW_NAMESPACE_END

#endif
