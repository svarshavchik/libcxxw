/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "tablelayoutmanager/tablelayoutmanager_impl.H"
#include "tablelayoutmanager/table_synchronized_axis.H"
#include "gridlayoutmanager.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

tablelayoutmanagerObj::implObj
::implObj(const ref<listcontainer_pseudo_implObj> &container_impl,
	  const ref<gridlayoutmanagerObj::implObj> &header_layoutmanager_impl,
	  const list_element &list_element_singleton,
	  const table_synchronized_axis &axis_impl,
	  const screen_positions_handleptr &config_handle)
	: listlayoutmanagerObj::implObj{container_impl,
					list_element_singleton},
	  axis_impl{axis_impl},
	  header_layoutmanager_impl{header_layoutmanager_impl},
	  config_handle{config_handle}
{
}

tablelayoutmanagerObj::implObj::~implObj()=default;

layoutmanager tablelayoutmanagerObj::implObj::create_public_object()
{
	return tablelayoutmanager::create(ref{this});
}

LIBCXXW_NAMESPACE_END
