/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_container_impl.H"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "popup/popup_attachedto_handler_element.H"
#include "x/w/impl/container_element.H"
#include "nonrecursive_visibility.H"

LIBCXXW_NAMESPACE_START

custom_combobox_containerObj::implObj
::implObj(const container_impl &parent_container,
	  const custom_combobox_popup_container &popup_container,
	  const ref<popup_attachedto_handlerObj> &attachedto_handler)
	: superclass_t(attachedto_handler, parent_container),
	  popup_container(popup_container)
{
}

custom_combobox_containerObj::implObj::~implObj()=default;

const char *custom_combobox_containerObj::implObj::label_theme_font() const
{
	return "combobox";
}

LIBCXXW_NAMESPACE_END
