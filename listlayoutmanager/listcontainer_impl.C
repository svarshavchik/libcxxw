/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listcontainer_impl.H"
#include "container_element.H"
#include "themedim_element.H"

LIBCXXW_NAMESPACE_START

listcontainerObj::implObj::implObj(const ref<containerObj::implObj> &parent,
				   const list_padding &padding)
	: listcontainer_impl_superclass_t(padding.v_padding,
					  padding.left_padding,
					  padding.inner_padding,
					  padding.right_padding,
					  parent)
{
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
