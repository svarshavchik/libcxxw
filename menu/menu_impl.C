/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_impl.H"
#include "nonrecursive_visibility.H"
#include "focus/focusframecontainer_element.H"
#include "popup/popup_attachedto_handler_element.H"
#include "container_element.H"

LIBCXXW_NAMESPACE_START

menuObj::implObj::implObj(const popup &menu_popup,
			  const ref<popup_attachedto_handlerObj>
			  &attachedto_handler,
			  const ref<containerObj::implObj> &parent_container)
	: superclass_t(attachedto_handler, parent_container,
		       child_element_init_params{"menufocusframe@libcxx"}),
	  menu_popup(menu_popup)
{
}

menuObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END