/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusframelayoutimpl.H"
#include "container.H"
#include "current_border_impl.H"
#include "element_screen.H"
#include "screen.H"
#include "grid_element.H"

LIBCXXW_NAMESPACE_START

focusframelayoutimplObj
::focusframelayoutimplObj(const ref<containerObj::implObj> &container_impl,
			  const char *off_border,
			  const char *on_border)
	: gridlayoutmanagerObj::implObj(container_impl),
	focusoff_border(container_impl->get_element_impl().get_screen()->impl
			->get_theme_border(off_border)),
	focuson_border(container_impl->get_element_impl().get_screen()->impl
		       ->get_theme_border(on_border))

{
}

focusframelayoutimplObj::~focusframelayoutimplObj()=default;

void focusframelayoutimplObj::rebuild_elements_start(IN_THREAD_ONLY,
						     grid_map_t::lock &lock)
{
	// Pick the border based on whether my container has input focus.

	auto correct_border=
		container_impl->get_element_impl()
		.current_keyboard_focus(IN_THREAD)
		? focuson_border:focusoff_border;

	// Should always be one element, here.

	for (const auto &row:lock->elements)
		for (const auto &col:row)
		{
			col->left_border=
				col->right_border=
				col->top_border=
				col->bottom_border=correct_border;
		}
}

LIBCXXW_NAMESPACE_END
