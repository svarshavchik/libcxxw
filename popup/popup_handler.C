/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
#include "popup/popup_attachedto_info.H"
#include "generic_window_handler.H"
#include "inherited_visibility_info.H"
#include "screen.H"
#include "connection_thread.H"
#include "grabbed_pointer.H"
#include "messages.H"
#include "shared_handler_data.H"
#include "icon.H"
#include "x/w/button_event.H"
#include "x/w/motion_event.H"
#include "x/w/key_event.H"
#include "x/w/main_window.H"
#include "x/w/impl/layoutmanager.H"
#include "x/w/impl/focus/focusable.H"

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

popupObj::handlerObj::handlerObj(const popup_handler_args &args)
	: handlerObj{args, {args.parent->screenref,
			    args.window_type,
			    args.window_state,
			    transparent,
			    args.appearance,
			}}
{
}

popupObj::handlerObj::handlerObj(const popup_handler_args &args,
				 const main_window_handler_constructor_params
				 &main_params)
	: superclass_t{{main_params,
			args.parent->handler_data,
			args.nesting_level,
			true}},
	  attachedto_info{args.attachedto_info},
	  attachedto_type{args.attachedto_type},
	  wm_class_instance{args.wm_class_instance},
	  popup_parent{args.parent}
{
}

popupObj::handlerObj::~handlerObj()=default;


void popupObj::handlerObj::installed(ONLY IN_THREAD)
{
	superclass_t::installed(IN_THREAD);

	auto pp=popup_parent.getptr();

	if (pp)
		pp->my_popups->push_back(ref(this));
}

main_windowptr popupObj::handlerObj::get_main_window()
{
	main_windowptr p;

	auto pp=popup_parent.getptr();

	if (pp)
		p=pp->get_main_window();

	return p;
}

void popupObj::handlerObj::set_default_wm_hints(ONLY IN_THREAD,
						xcb_icccm_wm_hints_t &hints)
{
	hints.flags=XCB_ICCCM_WM_HINT_INPUT;
	hints.input=1;
}

void popupObj::handlerObj
::absolute_location_updated(ONLY IN_THREAD,
			    absolute_location_update_reason reason)
{
	superclass_t::absolute_location_updated(IN_THREAD, reason);

	// The layoutmanager is toplevelpeephole_layoutmanagerObj.
	//
	// Its recalculate() checks the popup's current position, and updates
	// the popup's peepholed element's metrics to be no larger than
	// the popup's peephole, keeping it in sync. We need to make sure
	// recalculate() processes the popup's current position.

	invoke_layoutmanager([&]
			     (const auto &lm)
			     {
				     lm->needs_recalculation(IN_THREAD);
			     });
}

void popupObj::handlerObj::do_button_event(ONLY IN_THREAD,
					   const xcb_button_release_event_t *event,
					   const button_event &be,
					   const motion_event &me)
{
	if (activate_for(be) &&
	    (me.x < 0 || me.y < 0 ||
	     (dim_t::truncate)(me.x)
	     >= data(IN_THREAD).current_position.width ||
	     (dim_t::truncate)(me.y)
	     >= data(IN_THREAD).current_position.height))
	{
		request_visibility(IN_THREAD, false);
		return;
	}
	generic_windowObj::handlerObj::do_button_event(IN_THREAD, event, be,
						       me);
}

void popupObj::handlerObj::set_inherited_visibility_mapped(ONLY IN_THREAD)
{
	update_user_time(IN_THREAD);
	popup_opened(IN_THREAD);
	opened_mcguffin=((*handler_data).*(attachedto_type.opened_popup))
		(IN_THREAD, ref<popupObj::handlerObj>(this));
	superclass_t::set_inherited_visibility_mapped(IN_THREAD);
}


void popupObj::handlerObj::set_inherited_visibility_unmapped(ONLY IN_THREAD)
{
	superclass_t::set_inherited_visibility_unmapped(IN_THREAD);

	opened_mcguffin=nullptr;
	((*handler_data).*(attachedto_type.closed_popup))(IN_THREAD, *this);

	closing_popup(IN_THREAD);
	unset_keyboard_focus(IN_THREAD, {});
}

const char *popupObj::handlerObj::default_wm_class_instance() const
{
	return wm_class_instance;
}

std::string popupObj::handlerObj::default_wm_class_resource(ONLY IN_THREAD)
{
	auto p=popup_parent.getptr();

	if (p)
	{
		auto &parent=*p;

		if (parent.wm_class_resource(IN_THREAD).empty())
			return parent.default_wm_class_resource(IN_THREAD);

		return parent.wm_class_resource(IN_THREAD);
	}
	return superclass_t::default_wm_class_resource(IN_THREAD);
}

ptr<generic_windowObj::handlerObj>
popupObj::handlerObj::get_popup_parent(ONLY IN_THREAD)
{
	auto p=popup_parent.getptr();

	if (p)
		p=p->get_popup_parent(IN_THREAD);

	return p;
}

grabbed_pointerptr popupObj::handlerObj::grab_pointer(ONLY IN_THREAD,
						      const element_implptr &i)
{
	auto p=get_popup_parent(IN_THREAD);

	if (!p)
		return grabbed_pointerptr();

	return p->grab_pointer(IN_THREAD, i);
}

void popupObj::handlerObj::popup_opened(ONLY IN_THREAD)
{
	// Tooltips ignore button events and don't need the pointer grab.
	if (!popup_accepts_button_events(IN_THREAD))
		return;

	auto p=get_popup_parent(IN_THREAD);

	if (p)
	{
		auto mcguffin=p->grab_pointer(IN_THREAD, element_implptr());

		current_grab=mcguffin;
		if (mcguffin)
			mcguffin->allow_events(IN_THREAD);
	}
	else
	{
		LOG_ERROR("Popup parent does not exist");
	}
}

void popupObj::handlerObj::closing_popup(ONLY IN_THREAD)
{
	ungrab(IN_THREAD);
	current_grab=NULL;

	auto immediate_parent=popup_parent.getptr();

	if (immediate_parent)
	{
		auto autorestore_focus_to=
			immediate_parent->get_autorestorable_focusable();

		if (autorestore_focus_to)
			autorestore_focus_to->request_focus_quietly(IN_THREAD);
	}
}


bool popupObj::handlerObj::keep_passive_grab(ONLY IN_THREAD)
{
	auto p=popup_parent.getptr();

	if (p)
		return p->keep_passive_grab(IN_THREAD);

	return generic_windowObj::handlerObj::keep_passive_grab(IN_THREAD);
}

void popupObj::handlerObj::ungrab(ONLY IN_THREAD)
{
	auto p=popup_parent.getptr();

	if (p)
	{
		p->ungrab(IN_THREAD);
		return;
	}

	generic_windowObj::handlerObj::ungrab(IN_THREAD);
}

bool popupObj::handlerObj
::popup_accepts_key_events(ONLY IN_THREAD)
{
	return data(IN_THREAD).requested_visibility;
}

bool popupObj::handlerObj
::popup_accepts_button_events(ONLY IN_THREAD)
{
	return data(IN_THREAD).requested_visibility;
}

bool popupObj::handlerObj
::process_key_event(ONLY IN_THREAD, const key_event &ke)
{
	if (generic_windowObj::handlerObj::process_key_event(IN_THREAD, ke))
		return true;

	if (ke.keypress && ke.notspecial() && ke.unicode == '\e')
	{
		request_visibility(IN_THREAD, false);
		return true;
	}
	return false;
}

void popupObj::handlerObj
::update_attachedto_element_position(ONLY IN_THREAD,
				     const rectangle &new_position)
{
	auto &existing=attachedto_info->attachedto_element_position(IN_THREAD);

	if (existing == new_position)
		return;

	existing=new_position;

	// Need to recalculate out position, because it is based on the
	// attached_to element position.
	set_override_redirected_window_position(IN_THREAD);
}

void popupObj::handlerObj::recalculate_window_position(ONLY IN_THREAD,
						       rectangle &r,
						       dim_t screen_width,
						       dim_t screen_height)
{
	recalculate_attached_popup_position(IN_THREAD, r,
					    screen_width,
					    screen_height);
}

popup_position_affinity popupObj::handlerObj
::recalculate_attached_popup_position(ONLY IN_THREAD,
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
	case attached_to::tooltip:
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

LIBCXXW_NAMESPACE_END
