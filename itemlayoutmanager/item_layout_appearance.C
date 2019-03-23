/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/item_layout_appearance.H"

LIBCXXW_NAMESPACE_START

item_layout_appearance_properties::item_layout_appearance_properties()
	: itemlayout_h_padding{"itemlayout-h-padding"},
	  itemlayout_v_padding{"itemlayout-v-padding"}
{
}

item_layout_appearance_properties::~item_layout_appearance_properties()=default;

item_layout_appearanceObj::item_layout_appearanceObj()=default;

item_layout_appearanceObj::~item_layout_appearanceObj()=default;

item_layout_appearanceObj::item_layout_appearanceObj
(const item_layout_appearanceObj &o)
	: item_layout_appearance_properties{o}
{
}

item_layout_appearance item_layout_appearanceObj::clone() const
{
	return item_layout_appearance::create(*this);
}

const const_item_layout_appearance &item_layout_appearance_base::theme()
{
	static const const_item_layout_appearance config=
		const_item_layout_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
