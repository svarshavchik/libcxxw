/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "container_element.H"
#include "nonrecursive_visibility.H"

LIBCXXW_NAMESPACE_START

custom_combobox_containerObj::implObj
::implObj(const ref<containerObj::implObj> &parent_container,
	  const custom_combobox_popup_container &popup_container,
	  const ref<popup_attachedto_handlerObj> &attachedto_handler)
	: superclass_t(parent_container),
	  popup_container(popup_container),
	  attachedto_handler(attachedto_handler)
{
}

custom_combobox_containerObj::implObj::~implObj()=default;

const char *custom_combobox_containerObj::implObj::label_theme_font() const
{
	return "combobox";
}

void custom_combobox_containerObj::implObj::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	update_attachedto_info(IN_THREAD);
}

void custom_combobox_containerObj::implObj
::process_updated_position(IN_THREAD_ONLY)
{
	superclass_t::process_updated_position(IN_THREAD);
	update_attachedto_info(IN_THREAD);
}

void custom_combobox_containerObj::implObj
::absolute_location_updated(IN_THREAD_ONLY)
{
	superclass_t::absolute_location_updated(IN_THREAD);
	update_attachedto_info(IN_THREAD);
}

void custom_combobox_containerObj::implObj
::update_attachedto_info(IN_THREAD_ONLY)
{
	attachedto_handler->update_attachedto_element_position
		(IN_THREAD,
		 get_absolute_location_on_screen(IN_THREAD));
	popup_container->impl->needs_recalculation(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
