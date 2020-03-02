/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/combobox_appearance.H"
#include "x/w/focus_border_appearance.H"
#include <x/singleton.H>

LIBCXXW_NAMESPACE_START

combobox_appearance_properties::combobox_appearance_properties()
	: combobox_border{"combobox_border"},
	  button_focus_border{focus_border_appearance
			      ::base::combobox_button_theme()},
	  popup_button_image1{"scroll-down1"},
	  popup_button_image2{"scroll-down2"}
{
}

combobox_appearance_properties::~combobox_appearance_properties()=default;

combobox_appearanceObj::combobox_appearanceObj()=default;

combobox_appearanceObj::~combobox_appearanceObj()=default;

combobox_appearanceObj::combobox_appearanceObj(const combobox_appearanceObj &o)
	: popup_list_appearanceObj{o},
	  combobox_appearance_properties{o}
{
}

const_combobox_appearance combobox_appearanceObj
::do_modify(const function<void (const combobox_appearance &)> &closure) const
{
	auto copy=combobox_appearance::create(*this);
	closure(copy);
        return copy;
}

namespace {
#if 0
}
#endif

struct combobox_appearance_base_themeObj : virtual public obj {

	const const_combobox_appearance config=const_combobox_appearance::create();

};

#if 0
{
#endif
}

const_combobox_appearance combobox_appearance_base::theme()
{
	return singleton<combobox_appearance_base_themeObj>::get()->config;
}

LIBCXXW_NAMESPACE_END
