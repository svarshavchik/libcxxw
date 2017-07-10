/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/combobox_button_impl.H"

LIBCXXW_NAMESPACE_START

combobox_button_implObj
::combobox_button_implObj(const ref<containerObj::implObj> &container,
			  const std::vector<icon> &icon_images,
			  const ref<elementObj::implObj> &popup_element_impl)
	: image_button_internalObj::implObj(container, icon_images),
	popup_element_impl(popup_element_impl)
{
}

combobox_button_implObj::~combobox_button_implObj()=default;

void combobox_button_implObj
::temperature_changed(IN_THREAD_ONLY)
{
	set_image_number(IN_THREAD,
			 hotspot_temperature(IN_THREAD)
			 == temperature::hot ? 1:0);
}

void combobox_button_implObj::activated(IN_THREAD_ONLY)
{
	popup_element_impl
		->request_visibility(IN_THREAD,
				     !popup_element_impl->data(IN_THREAD)
				     .requested_visibility);

	image_button_internalObj::implObj::activated(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
