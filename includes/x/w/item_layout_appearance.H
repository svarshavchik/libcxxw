/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_item_layout_appearance_h
#define x_w_item_layout_appearance_h

#include <x/w/item_layout_appearancefwd.H>
#include <x/w/item_layout_appearanceobj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Base class for a \ref item_layout_appearance "item_layout appearance" specification.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::item_layout_appearance::base.

class item_layout_appearance_base : public ptrref_base {

public:

	//! Default theme.

	static const_item_layout_appearance theme();

};

LIBCXXW_NAMESPACE_END

#endif
