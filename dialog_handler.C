/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "dialog_handler.H"
#include "connection_thread.H"
#include "inherited_visibility_info.H"
#include "shared_handler_data.H"
#include "screen.H"
#include "connection.H"
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START

dialogObj::handlerObj::handlerObj(const ref<main_windowObj::handlerObj>
				  &parent_handler,
				  const std::variant<dialog_position,
				  rectangle> &position,
				  const std::string &window_id,
				  const color_arg &background_color,
				  const font_arg &default_label_font,
				  const color_arg &default_label_color,
				  const char *window_type,
				  bool modal,
				  bool urgent,
				  bool grab_input_focus)
	: superclass_t{{parent_handler->screenref,
			window_type,
			modal ? "MODAL":"",
			background_color,
			default_label_font,
			default_label_color,
			},
		       std::visit(visitor{[&](const rectangle &r)
					  -> std::optional<rectangle>
					  {
					   return r;
					  },
					  [](const auto &)
					  -> std::optional<rectangle>
					  {
					   return std::nullopt;
					  }}, position),
		       window_id,
		       false},
	  my_position_thread_only
	{
	 std::visit(visitor{[&](const dialog_position &pos)
			    {
				    return pos;
			    },
			    [](const auto &)
			    {
				    return dialog_position::default_position;
			    }}, position)
	},
	  modal{modal},
	  parent_handler{parent_handler},
	  urgent_dialog{urgent},
	  grab_input_focus{grab_input_focus},
	  urgent_thread_only{urgent}
{
}

dialogObj::handlerObj::~handlerObj()=default;

const char *dialogObj::handlerObj::default_wm_class_instance() const
{
	return "dialog";
}

void dialogObj::handlerObj
::set_default_wm_hints(ONLY IN_THREAD,
		       xcb_icccm_wm_hints_t &hints)
{
	superclass_t::set_default_wm_hints(IN_THREAD, hints);

	if (urgent(IN_THREAD))
		hints.flags |= XCB_ICCCM_WM_HINT_X_URGENCY;
}

void dialogObj::handlerObj
::button_press_event(ONLY IN_THREAD,
		     const xcb_button_press_event_t *event)
{
	if (urgent(IN_THREAD))
	{
		urgent(IN_THREAD)=false;
		update_wm_hints(IN_THREAD);
	}

	superclass_t::button_press_event(IN_THREAD, event);
}

void dialogObj::handlerObj::key_press_event(ONLY IN_THREAD,
					    const xcb_key_press_event_t *event,
					    uint16_t sequencehi)
{
	if (urgent(IN_THREAD))
	{
		urgent(IN_THREAD)=false;
		update_wm_hints(IN_THREAD);
	}
	superclass_t::key_press_event(IN_THREAD, event, sequencehi);
}

bool dialogObj::handlerObj::handle_our_own_placement(ONLY IN_THREAD)
{
	return !get_screen()->supported("_NET_WM_FULL_PLACEMENT") ||
		my_position(IN_THREAD) != dialog_position::default_position;
}

void dialogObj::handlerObj::set_inherited_visibility_mapped(ONLY IN_THREAD)
{
	if (modal)
		acquired_busy_mcguffin(IN_THREAD)=
			parent_handler->get_shade_busy_mcguffin();

	parent_handler->handler_data->opening_dialog(IN_THREAD);

	if (urgent(IN_THREAD) != urgent_dialog)
	{
		urgent(IN_THREAD)=urgent_dialog; // Reset when reopening.
		update_wm_hints(IN_THREAD);
	}

	if (grab_input_focus)
		update_user_time(IN_THREAD);
	else
	{
		update_user_time(IN_THREAD, 0);
	}
	superclass_t::set_inherited_visibility_mapped(IN_THREAD);
}

xcb_size_hints_t dialogObj::handlerObj::compute_size_hints(ONLY IN_THREAD)
{
	auto hints=superclass_t::compute_size_hints(IN_THREAD);

	if (!handle_our_own_placement(IN_THREAD))
		return hints;

	if (hints.flags & XCB_ICCCM_SIZE_HINT_US_POSITION)
		return hints; // Overridden in the superclass.

	// Before we become visible we are going to
	// manually position the dialog so that it's
	// centered over its parent window.

	rectangle parent_pos=
		parent_handler->current_position.get();

	parent_pos.x=0;
	parent_pos.y=0;
	parent_handler->get_absolute_location_on_screen(IN_THREAD,
							parent_pos);

	auto &workarea=frame_extents(IN_THREAD).workarea;

	auto width=preferred_width(IN_THREAD);
	auto height=preferred_height(IN_THREAD);

	coord_t placement_x=parent_pos.x;
	coord_t placement_y=parent_pos.y;

	switch (my_position(IN_THREAD)) {
	case dialog_position::on_the_left:
		{
			dim_t sub_x=dim_t::truncate
				(parent_handler->frame_extents(IN_THREAD).left +
				 frame_extents(IN_THREAD).right + width);
			placement_x=coord_t::truncate
				(placement_x-sub_x);
		}
		break;
	case dialog_position::on_the_right:
		{
			dim_t add_x=dim_t::truncate
				(parent_handler->
				 frame_extents(IN_THREAD).right +
				 frame_extents(IN_THREAD).left +
				 parent_pos.width);
			placement_x=coord_t::truncate
				(placement_x+add_x);
		}
		break;
	case dialog_position::above:
		{
			dim_t sub_y=dim_t::truncate
				(parent_handler->frame_extents(IN_THREAD).top +
				 frame_extents(IN_THREAD).bottom + height);
			placement_y=coord_t::truncate
				(placement_y-sub_y);
		}
		break;
	case dialog_position::below:
		{
			dim_t add_y=dim_t::truncate
				(parent_handler->
				 frame_extents(IN_THREAD).bottom +
				 frame_extents(IN_THREAD).top + height);
			placement_y=coord_t::truncate(placement_y+add_y);
		}
		break;
	default:
		coord_t parent_center_x=
			coord_t::truncate(parent_pos.x + parent_pos.width/2);

		coord_t parent_center_y=
			coord_t::truncate(parent_pos.y + parent_pos.height/2);

		placement_x=coord_t::truncate(parent_center_x-width/2);
		placement_y=coord_t::truncate(parent_center_y-height/2);

		break;
	};

	// Make sure we are not placed outside of the workarea

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

	xcb_icccm_size_hints_set_position(&hints, 0,
					  coord_t::truncate(placement_x),
					  coord_t::truncate(placement_y));
	xcb_icccm_size_hints_set_size(&hints, 0,
				      dim_t::truncate(width),
				      dim_t::truncate(height));

	xcb_icccm_size_hints_set_win_gravity(&hints,
					     XCB_GRAVITY_STATIC);

	return hints;
}

void dialogObj::handlerObj::set_inherited_visibility_unmapped(ONLY IN_THREAD)
{
	superclass_t::set_inherited_visibility_unmapped(IN_THREAD);
	parent_handler->handler_data->closing_dialog(IN_THREAD);
	acquired_busy_mcguffin(IN_THREAD)=nullptr;
	my_position(IN_THREAD)=dialog_position::default_position;
}

std::string dialogObj::handlerObj::default_wm_class_resource(ONLY IN_THREAD)
{
	if (parent_handler->wm_class_resource(IN_THREAD).empty())
		return parent_handler->default_wm_class_resource(IN_THREAD);

	return parent_handler->wm_class_resource(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
