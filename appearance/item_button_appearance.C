/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/item_button_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

item_button_appearance_properties::item_button_appearance_properties()
	: button_border{"itembutton_border"},
	  itembutton_h_padding{"itembutton-h-padding"},
	  itembutton_v_padding{"itembutton-v-padding"},
	  itembutton_background_color{"itembutton_background_color"},
	  item_image_button_appearance{image_button_appearance::base
			  ::item_theme()}

{
}

item_button_appearance_properties::~item_button_appearance_properties()=default;

item_button_appearanceObj::item_button_appearanceObj()=default;

item_button_appearanceObj::~item_button_appearanceObj()=default;

item_button_appearanceObj::item_button_appearanceObj
(const item_button_appearanceObj &o)
	: item_button_appearance_properties{o}
{
}

const_item_button_appearance item_button_appearanceObj
::do_modify(const function<void (const item_button_appearance &)> &closure) const
{
	auto copy=item_button_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct item_button_appearance_base_themeObj : virtual public obj {

	const const_item_button_appearance config=const_item_button_appearance::create();

};

#if 0
{
#endif
}

const_item_button_appearance item_button_appearance_base::theme()
{
	return singleton<item_button_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
