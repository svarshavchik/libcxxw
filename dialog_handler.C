/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dialog_handler.H"
#include "connection_thread.H"

LIBCXXW_NAMESPACE_START

dialogObj::handlerObj::handlerObj(IN_THREAD_ONLY,
				  const ref<generic_windowObj
				  ::handlerObj> &parent_handler,
				  const color_arg &background_color,
				  bool modal)
	: main_windowObj::handlerObj(IN_THREAD, parent_handler->get_screen(),
				     background_color),
	modal(modal),
	parent_handler(parent_handler)
{
	update_user_time(IN_THREAD);
}

dialogObj::handlerObj::~handlerObj()=default;

const char *dialogObj::handlerObj::default_wm_class_instance() const
{
	return "dialog";
}

bool dialogObj::handlerObj::handle_our_own_placement()
{
	return !get_screen()->supported("_NET_WM_FULL_PLACEMENT");
}

void dialogObj::handlerObj
::set_inherited_visibility(IN_THREAD_ONLY,
			   inherited_visibility_info &visibility_info)
{
	if (visibility_info.flag && modal)
		acquired_busy_mcguffin(IN_THREAD)=
			parent_handler->get_busy_mcguffin();

	if (visibility_info.flag && handle_our_own_placement())
	{
		// Before we become visible we are going to
		// manually position the dialog so that it's
		// centered over its parent window.

		rectangle parent_pos=
			parent_handler->get_absolute_location(IN_THREAD);

		parent_pos.x=0;
		parent_pos.y=0;
		parent_handler->get_absolute_location_on_screen(IN_THREAD,
								parent_pos);

		auto parent_center_x=
			coord_squared_t::truncate(parent_pos.x +
						  parent_pos.width/2);

		auto parent_center_y=
			coord_squared_t::truncate(parent_pos.y +
						  parent_pos.height/2);

		mpobj<rectangle>::lock lock{current_position};

		coord_t placement_x=coord_t::truncate
			(parent_center_x-dim_t::truncate(lock->width/2));
		coord_t placement_y=coord_t::truncate
			(parent_center_y-dim_t::truncate(lock->height/2));

		auto workarea=get_screen()->get_workarea();

		coord_t right=coord_t::truncate(workarea.x+workarea.width-
						dim_t::truncate
						(data(IN_THREAD)
						 .current_position.width));
		coord_t bottom=coord_t::truncate(workarea.y+workarea.height-
						 dim_t::truncate
						 (data(IN_THREAD)
						  .current_position.height));

		if (placement_x > right)
			placement_x=right;

		if (placement_y > bottom)
			placement_y=bottom;

		if (placement_x < workarea.x)
			placement_x=workarea.x;

		if (placement_y < workarea.y)
			placement_y=workarea.y;

		values_and_mask configure_window_vals
			(XCB_CONFIG_WINDOW_X,
			 (coord_t::value_type)placement_x,

			 XCB_CONFIG_WINDOW_Y,
			 (coord_t::value_type)placement_y);

		xcb_configure_window(IN_THREAD->info->conn, id(),
				     configure_window_vals.mask(),
				     configure_window_vals.values().data());
	}

	main_windowObj::handlerObj::set_inherited_visibility(IN_THREAD,
							     visibility_info);
	if (!visibility_info.flag)
		acquired_busy_mcguffin(IN_THREAD)=nullptr;
}

xcb_size_hints_t dialogObj::handlerObj::compute_size_hints(IN_THREAD_ONLY)
{
	auto hints=main_windowObj::handlerObj::compute_size_hints(IN_THREAD);

	if (handle_our_own_placement())
		hints.flags |= XCB_ICCCM_SIZE_HINT_P_POSITION;

	return hints;
}

std::string dialogObj::handlerObj::default_wm_class_resource(IN_THREAD_ONLY)
{
	if (parent_handler->wm_class_resource(IN_THREAD).empty())
		return parent_handler->default_wm_class_resource(IN_THREAD);

	return parent_handler->wm_class_resource(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
