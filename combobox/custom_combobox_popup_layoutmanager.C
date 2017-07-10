/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "combobox/custom_combobox_popup_layoutmanager.H"
#include "combobox/custom_combobox_popup_container_impl.H"
#include "gridlayoutmanager_impl_elements.H"

LIBCXXW_NAMESPACE_START

custom_combobox_popup_layoutmanagerObj
::custom_combobox_popup_layoutmanagerObj
(const ref<custom_combobox_popup_containerObj::implObj> &container_impl,
 const new_listlayoutmanager &style)
	: listlayoutmanagerObj::implObj(container_impl, style),
	container_impl(container_impl)
{
}

custom_combobox_popup_layoutmanagerObj
::~custom_combobox_popup_layoutmanagerObj()=default;

void custom_combobox_popup_layoutmanagerObj
::process_updated_position(IN_THREAD_ONLY,
			   const rectangle &position)
{
	listlayoutmanagerObj::implObj::process_updated_position(IN_THREAD,
								position);

	dim_t tallest=0;

	for (const auto &r:grid_elements(IN_THREAD)->vert_sizes)
	{
		dim_t height=std::get<dim_t>(r.second);

		if (height > tallest)
			tallest=height;
	}

	container_impl->update_tallest_row_height(IN_THREAD, tallest);
}

LIBCXXW_NAMESPACE_END
