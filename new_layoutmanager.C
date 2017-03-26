/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "new_layoutmanager.H"
#include "container.H"
#include "layoutmanager.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

new_layoutmanager::new_layoutmanager()=default;

new_layoutmanager::~new_layoutmanager()=default;

new_gridlayoutmanager::new_gridlayoutmanager()=default;

new_gridlayoutmanager::~new_gridlayoutmanager()=default;

new_layoutmanager_results
new_gridlayoutmanager::create(const new_layoutmanager_info &info) const
{
	return {
		ref<gridlayoutmanagerObj::implObj>::create(info.container_impl),
			};
}

LIBCXXW_NAMESPACE_END
