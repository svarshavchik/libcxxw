/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/combobox_button_impl.H"
#include "popup/popup_showhide_element.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

combobox_button_implObj
::combobox_button_implObj(const ref<containerObj::implObj> &container,
			  const std::vector<icon> &icon_images,
			  const ref<elementObj::implObj> &popup_element_impl)
	: superclass_t(popup_element_impl, container, icon_images)
{
}

combobox_button_implObj::~combobox_button_implObj()=default;

void combobox_button_implObj
::temperature_changed(IN_THREAD_ONLY,
		      const callback_trigger_t &trigger)
{
	set_image_number(IN_THREAD,
			 trigger,
			 hotspot_temperature(IN_THREAD)
			 == temperature::hot ? 1:0);
}

bool combobox_button_implObj::activate_on_key(const key_event &ke)
{
	return superclass_t::activate_on_key(ke)
		|| ke.keysym == XK_Down || ke.keysym == XK_KP_Down;
}

LIBCXXW_NAMESPACE_END
