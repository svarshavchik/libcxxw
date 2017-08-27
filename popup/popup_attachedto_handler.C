/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_attachedto_info.H"
#include "connection_thread.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

popup_attachedto_handlerObj
::popup_attachedto_handlerObj(opened_popup_t opened_popup,
			      closed_popup_t closed_popup,
			      const ref<generic_windowObj::handlerObj> &parent,
			      const popup_attachedto_info &attachedto_info,
			      size_t nesting_level)
	: popupObj::handlerObj(parent->thread(), parent, nesting_level),
	attachedto_info(attachedto_info),
	opened_popup(opened_popup),
	closed_popup(closed_popup)
{
}

popup_attachedto_handlerObj::~popup_attachedto_handlerObj()=default;

void popup_attachedto_handlerObj
::update_attachedto_element_position(IN_THREAD_ONLY,
				     const rectangle &new_position)
{
	attachedto_info->attachedto_element_position(IN_THREAD)=new_position;
	set_popup_position(IN_THREAD);
}


void popup_attachedto_handlerObj
::recalculate_popup_position(IN_THREAD_ONLY,
			     rectangle &r,
			     dim_t screen_width,
			     dim_t screen_height)
{
	auto max_peephole_width_value=
		attachedto_info->max_peephole_width(IN_THREAD, screenref);
	auto max_peephole_height_value=
		attachedto_info->max_peephole_height(IN_THREAD, screenref);

	auto &attachedto_element_position=
		attachedto_info->attachedto_element_position(IN_THREAD);

	if (r.width > max_peephole_width_value)
		r.width=max_peephole_width_value;

	if (r.height > max_peephole_height_value)
		r.height=max_peephole_height_value;

	if (attachedto_info->how == attached_to::submenu_next)
	{
		// The popup cannot start to the right of max_x, without
		// getting truncated.
		coord_t max_x=coord_t::truncate(screen_width - r.width);

		// Here's where the popup will start.
		coord_t x=coord_t::truncate(attachedto_element_position.x +
					    attachedto_element_position.width);

		if (x > max_x)
			x=coord_t::truncate(attachedto_element_position.x -
					    r.width);

		// The popup's y position is same as element's, but it
		// cannot be below max_y.
		coord_t max_y=coord_t::truncate(screen_height - r.height);
		coord_t y=attachedto_element_position.y;

		if (y > max_y)
			y=max_y;

		r.x=x;
		r.y=y;
	}
	else
	{
		// It'll be above or below, but start on the same x coordinate
		// as the attached to element, but not to the right of max_x.

		coord_t max_x=coord_t::truncate(screen_width - r.width);
		coord_t x=attachedto_element_position.x;

		if (x > max_x)
			x=max_x;

		// If the popup start above max_y, there's enough room for it.

		coord_t max_y=coord_t::truncate(screen_height - r.height);
		coord_t y=coord_t::truncate(attachedto_element_position.y +
					    attachedto_element_position.height
					    );

		if (y > max_y)
			y=coord_t::truncate(attachedto_element_position.y
					    - r.height);

		r.x=x;
		r.y=y;

		// The width shall be equal to the combobox's container.

		if (r.width < attachedto_element_position.width)
			r.width=attachedto_element_position.width;
	}
}

ref<obj> popup_attachedto_handlerObj::get_opened_mcguffin(IN_THREAD_ONLY)
{
	return ((*handler_data).*opened_popup)(IN_THREAD,
					       ref<popupObj::handlerObj>(this)
					       );
}

void popup_attachedto_handlerObj::released_opened_mcguffin(IN_THREAD_ONLY)
{
	((*handler_data).*closed_popup)(IN_THREAD, *this);
}

LIBCXXW_NAMESPACE_END
