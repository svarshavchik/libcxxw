/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "element.H"

LIBCXXW_NAMESPACE_START

listitemcontainerObj::implObj
::implObj(const ref<listcontainerObj::implObj> &parent_container,
	  const child_element_init_params &params)
	: superclass_t(parent_container, params),
	  parent_container(parent_container)
{
}

listitemcontainerObj::implObj::~implObj()=default;

void listitemcontainerObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	superclass_t::pointer_focus(IN_THREAD);

	parent_container->pointer_focus(IN_THREAD,
					ref<elementObj::implObj>(this));
}


bool listitemcontainerObj::implObj::selected() const
{
	mpobj<bool>::lock lock{flag};

	return *lock;
}

void listitemcontainerObj::implObj::selected(bool v)
{
	mpobj<bool>::lock lock{flag};

	*lock=v;
}

LIBCXXW_NAMESPACE_END
