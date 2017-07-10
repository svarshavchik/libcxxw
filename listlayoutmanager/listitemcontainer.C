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
		       const ref<listitemlayoutmanagerObj::implObj> &l,
		       const std::function<list_item_status_change_callback_t>
		       &status_change_callback)
	: containerObj(impl, l),
	  impl(impl),
	  status_change_callback(status_change_callback)
{
}

listitemcontainerObj::~listitemcontainerObj()=default;

LIBCXXW_NAMESPACE_END
