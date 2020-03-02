/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/item_layout_appearance.H"
#include "x/w/scrollbar_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

item_layout_appearance_properties::item_layout_appearance_properties()
	: itemlayout_h_padding{"itemlayout-h-padding"},
	  itemlayout_v_padding{"itemlayout-v-padding"},
	  horizontal_scrollbar{scrollbar_appearance::base::theme()},
	  vertical_scrollbar{scrollbar_appearance::base::theme()}
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

const_item_layout_appearance item_layout_appearanceObj
::do_modify(const function<void (const item_layout_appearance &)> &closure) const
{
	auto copy=item_layout_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct item_layout_appearance_base_themeObj : virtual public obj {

	const const_item_layout_appearance config=const_item_layout_appearance::create();

};

#if 0
{
#endif
}

const_item_layout_appearance item_layout_appearance_base::theme()
{
	return singleton<item_layout_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
