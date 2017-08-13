/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/firstlistitemcontainer.H"

LIBCXXW_NAMESPACE_START

firstlistitemcontainerObj
::firstlistitemcontainerObj(const ref<implObj> &impl,
			    const ref<listitemlayoutmanagerObj::implObj> &l,
			    const std::function<
			    list_item_status_change_callback_t
			    > &status_change_callback)
	: listitemcontainerObj(impl, l),
	  status_change_callback(status_change_callback)
{
}

firstlistitemcontainerObj::~firstlistitemcontainerObj()=default;

LIBCXXW_NAMESPACE_END
