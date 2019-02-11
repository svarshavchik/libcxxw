/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_attachedto_info.H"
#include "connection_thread.H"
#include "xid_t.H"

LIBCXXW_NAMESPACE_START

//! Specifies popup semantics.
struct LIBCXX_HIDDEN popup_visibility_semantics {

	//! Invoke this method when the popup becomes visible.

	ref<obj> (shared_handler_dataObj::*opened_popup)
		(ONLY IN_THREAD, const ref<popupObj::handlerObj> &);

	//! Invoke this method when the popup is no longer visible.

	void (shared_handler_dataObj::*closed_popup)
		(ONLY IN_THREAD, const popupObj::handlerObj &);
};

const popup_visibility_semantics exclusive_popup_type={
	&shared_handler_dataObj::opening_exclusive_popup,
	&shared_handler_dataObj::closing_exclusive_popup
};

const popup_visibility_semantics menu_popup_type={
	&shared_handler_dataObj::opening_menu_popup,
	&shared_handler_dataObj::closing_menu_popup
};

popup_attachedto_handlerObj
::popup_attachedto_handlerObj(const popup_attachedto_handler_args &args)
	: popupObj::handlerObj{args.parent->thread(), args.parent,
		"transparent",
		args.nesting_level},
	attachedto_info{args.attachedto_info},
	attachedto_type{args.attachedto_type},
	wm_class_instance{args.wm_class_instance}
{
}

popup_attachedto_handlerObj::~popup_attachedto_handlerObj()=default;

const char *popup_attachedto_handlerObj::default_wm_class_instance() const
{
	return wm_class_instance;
}

void popup_attachedto_handlerObj
::update_attachedto_element_position(ONLY IN_THREAD,
				     const rectangle &new_position)
{
	auto &existing=attachedto_info->attachedto_element_position(IN_THREAD);

	if (existing == new_position)
		return;

	existing=new_position;
	set_popup_position(IN_THREAD);
}

popup_position_affinity popup_attachedto_handlerObj
::recalculate_popup_position(ONLY IN_THREAD,
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

	popup_position_affinity a=popup_position_affinity::right;

	// The popup cannot start to the right of max_x, without
	// getting truncated.
	coord_t max_x=coord_t::truncate(screen_width - r.width);

	// The popup cannot be below max_y.
	coord_t max_y=coord_t::truncate(screen_height - r.height);

	coord_t x=attachedto_element_position.x;
	coord_t y=attachedto_element_position.y;

	switch (attachedto_info->how) {
	case attached_to::right_or_left:

		// Here's where the popup will start.
		x=coord_t::truncate(x+attachedto_element_position.width);

		// a=popup_position_affinity::right;

		if (x > max_x)
		{
			x=coord_t::truncate(attachedto_element_position.x -
					    r.width);
			a=popup_position_affinity::left;
		}

		// The popup's y position is same as element's

		if (y > max_y)
			y=max_y;

		break;

	case attached_to::below_or_above:
		// It'll be above or below, but start on the same x coordinate
		// as the attached to element, but not to the right of max_x.

		if (x > max_x)
			x=max_x;

		// If the popup start above max_y, there's enough room for it.

		y=coord_t::truncate(y + attachedto_element_position.height);

		a=popup_position_affinity::below;

		if (y > max_y)
		{
			a=popup_position_affinity::above;
			y=coord_t::truncate(attachedto_element_position.y
					    - r.height);
		}

		// This positioning is being used for combo-boxes and menus.
		// For combo-boxes we want the width to always be at least
		// as wide as the display element we're attached to.
		//
		// Menu popups hitch along for this ride...

		if (r.width < attachedto_element_position.width)
			r.width=attachedto_element_position.width;
		break;

	case attached_to::above_or_below:
		// It'll be above or below, but start on the same x coordinate
		// as the attached to element, but not to the right of max_x.

		if (x > max_x)
			x=max_x;

		// If the popup start above max_y, there's enough room for it.

		y=coord_t::truncate(y - r.height);

		a=popup_position_affinity::above;

		if (y < 0)
		{
			a=popup_position_affinity::below;
			y=coord_t::truncate(attachedto_element_position.y +
					    attachedto_element_position.height);
		}

		// This positioning is being used for combo-boxes and menus.
		// For combo-boxes we want the width to always be at least
		// as wide as the display element we're attached to.
		//
		// Menu popups hitch along for this ride...

		if (r.width < attachedto_element_position.width)
			r.width=attachedto_element_position.width;
		break;
	}

	r.x=x;
	r.y=y;
	return a;
}

ref<obj> popup_attachedto_handlerObj::get_opened_mcguffin(ONLY IN_THREAD)
{
	return ((*handler_data).*(attachedto_type.opened_popup))
		(IN_THREAD, ref<popupObj::handlerObj>(this));
}

void popup_attachedto_handlerObj::released_opened_mcguffin(ONLY IN_THREAD)
{
	((*handler_data).*(attachedto_type.closed_popup))(IN_THREAD, *this);
}

LIBCXXW_NAMESPACE_END
