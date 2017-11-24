/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframelayoutimpl.H"
#include "focus/focusframecontainer_impl.H"
#include "container.H"
#include "current_border_impl.H"
#include "element_screen.H"
#include "screen.H"
#include "grid_element.H"
#include "grid_map_info.H"

LIBCXXW_NAMESPACE_START

focusframelayoutimplObj
::focusframelayoutimplObj(const ref<focusframecontainerObj::implObj>
			  &container_impl)
	: gridlayoutmanagerObj::implObj(ref(&container_impl
					    ->get_container_impl())),
	container_impl{container_impl}
{
	requested_col_width(0, 100);
	requested_row_height(0, 100);
	col_alignment(0, halign::fill);
	row_alignment(0, valign::fill);
}

focusframelayoutimplObj::~focusframelayoutimplObj()=default;

void focusframelayoutimplObj::rebuild_elements_start(IN_THREAD_ONLY,
						     grid_map_t::lock &lock)
{
	// Pick the border based on whether my container has input focus.

	auto correct_border=
		container_impl->get_container_impl().get_element_impl()
		.current_keyboard_focus(IN_THREAD)
		? container_impl->get_focuson_border()
		: container_impl->get_focusoff_border();

	// Should always be one element, here.

	for (const auto &row:(*lock)->elements)
		for (const auto &col:row)
		{
			col->left_border=
				col->right_border=
				col->top_border=
				col->bottom_border=correct_border;
		}
}

LIBCXXW_NAMESPACE_END
