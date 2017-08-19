/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemcontainer_impl.H"
#include "listlayoutmanager/listitemlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

listitemcontainerObj
::listitemcontainerObj(const ref<implObj> &impl,
		       const ref<listitemlayoutmanagerObj::implObj> &l)
	: containerObj(impl, l),
	  impl(impl)
{
}

listitemcontainerObj::~listitemcontainerObj()=default;

element listitemcontainerObj::get()
{
	elementptr e;

	impl->invoke_layoutmanager
		([&]
		 (const ref<listitemlayoutmanagerObj::implObj> &l)
		 {
			 e=*listitemlayoutmanagerObj::implObj
				 ::current_element_t::lock{
				 l->current_element
			 };
		 });

	return e;
}

LIBCXXW_NAMESPACE_END
