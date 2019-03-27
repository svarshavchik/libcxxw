/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/combobox_appearance.H"

LIBCXXW_NAMESPACE_START

combobox_appearance_properties::combobox_appearance_properties()
	: combobox_border{"combobox_border"},
	  combobox_button_focusoff_border{"comboboxbuttonfocusoff_border"},
	  combobox_button_focuson_border{"comboboxbuttonfocuson_border"}
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

const const_combobox_appearance &combobox_appearance_base::theme()
{
	static const const_combobox_appearance config=
		const_combobox_appearance::create();

	return config;
}

LIBCXXW_NAMESPACE_END
