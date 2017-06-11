/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "container_element.H"
#include "themedim_element.H"
#include "x/w/listlayoutmanager.H"
#include "focus/focusable_element.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::implObj::implObj(const ref<containerObj::implObj> &parent,
				   const new_listlayoutmanager &style)
	: listcontainer_impl_superclass_t(style.v_padding,
					  style.left_padding,
					  style.inner_padding,
					  style.right_padding,
					  parent)
{
	if (style.columns < 1)
		throw EXCEPTION(_("Cannot create a list with 0 columns"));
}


listcontainerObj::implObj::~implObj()=default;

ref<containerObj::implObj>
listcontainerObj::implObj::parent_for_new_child(const ref<containerObj::implObj>
						&me)
{
	child_element_init_params init_params;

	init_params.container_override=true;

	return ref<container_elementObj<child_elementObj>>
		::create(ref<containerObj::implObj>(this), init_params);
}

LIBCXXW_NAMESPACE_END
