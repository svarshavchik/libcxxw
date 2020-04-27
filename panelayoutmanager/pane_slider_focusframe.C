/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/pane_slider_focusframe.H"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/container_visible_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/themeborder_element.H"
#include "cursor_pointer_element.H"
#include "grabbed_pointer.H"
#include "x/w/button_event.H"
#include "x/w/key_event.H"
#include "x/w/motion_event.H"
#include "x/w/pane_layout_appearance.H"
#include "x/w/focus_border_appearance.H"
#include "focus/label_for.H"
#include <X11/keysym.h>

LIBCXXW_NAMESPACE_START

pane_slider_focusframeObj
::pane_slider_focusframeObj(const container_impl &parent,
			    const const_pane_layout_appearance &appearance,
			    const cursor_pointer &custom_pointer)
	: superclass_t{custom_pointer,
		       appearance->slider_focus_border,
		       0,
		       0,
		       parent,
		       parent,
		       child_element_init_params
		       {FOCUSFRAME_SCRATCH_BUFFER_ID,
			{},
			appearance->slider_background_color}}
{
}

pane_slider_focusframeObj::~pane_slider_focusframeObj()=default;

// Update the cursor pointer.
void pane_slider_focusframeObj::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	set_cursor_pointer(IN_THREAD, tagged_cursor_pointer(IN_THREAD));
}

void pane_slider_focusframeObj::theme_updated(ONLY IN_THREAD,
					      const const_defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	set_cursor_pointer(IN_THREAD, tagged_cursor_pointer(IN_THREAD));
}

// Button 1 starts the sliding process.

bool pane_slider_focusframeObj::process_button_event(ONLY IN_THREAD,
						     const button_event &be,
						     xcb_timestamp_t timestamp)
{
	// Capture the redirect flag before forwarding process_button_event,
	// because the redirect flag can get set in there, and then we
	// get here. This is the only right sequence for this.
	bool was_redirected=be.redirect_info.event_redirected;
	bool flag=superclass_t::process_button_event(IN_THREAD, be, timestamp);

	if (be.button == 1 && !was_redirected)
	{
		if (be.press)
		{
			grabbed_x=coord_t::truncate
				(data(IN_THREAD).last_motion_x
				 + data(IN_THREAD).current_position.x);
			grabbed_y=coord_t::truncate
				(data(IN_THREAD).last_motion_y
				 + data(IN_THREAD).current_position.y);

			child_container->invoke_layoutmanager
				([&, this]
				 (const ref<panelayoutmanagerObj::implObj> &lm)
				 {
					 grid_map_t::lock grid_lock
						 {lm->grid_map};
					 original_sizes=lm->start_sliding
						 (IN_THREAD, ref{this},
						  grid_lock);
				 });
			grab(IN_THREAD);
		}
		else
		{
			original_sizes.reset();
		}
		flag=true;
	}

	return flag;
}

void pane_slider_focusframeObj::pointer_focus(ONLY IN_THREAD,
					      const callback_trigger_t &trigger)
{
	if (!current_pointer_focus(IN_THREAD))
		original_sizes.reset(); // No more sliding.
}

// Keyboard-based sliding.

bool pane_slider_focusframeObj::process_key_event(ONLY IN_THREAD,
						  const key_event &ke)
{
	switch (ke.keysym) {
	case XK_Up:
	case XK_KP_Up:
	case XK_Left:
	case XK_KP_Left:
		if (ke.keypress)
			child_container->invoke_layoutmanager
				([&, this]
				 (const ref<panelayoutmanagerObj::implObj> &lm)
				 {
					 lm->slide_start(IN_THREAD, ref(this));
				 });
		return true;
	case XK_Down:
	case XK_KP_Down:
	case XK_Right:
	case XK_KP_Right:
		if (ke.keypress)
			child_container->invoke_layoutmanager
				([&, this]
				 (const ref<panelayoutmanagerObj::implObj> &lm)
				 {
					 lm->slide_end(IN_THREAD, ref(this));
				 });
		return true;
	}

	return superclass_t::process_key_event(IN_THREAD, ke);
}

// Motion events while sliding adjust the relative sizes of the panes.

void pane_slider_focusframeObj::report_motion_event(ONLY IN_THREAD,
						    const motion_event &me)
{
	superclass_t::report_motion_event(IN_THREAD, me);

	if ((me.mask.buttons & 1) && original_sizes)
	{
		child_container->invoke_layoutmanager
			([&, this]
			 (const ref<panelayoutmanagerObj::implObj> &lm)
			 {
				 coord_t x=coord_t::truncate
					 (me.x
					  + data(IN_THREAD).current_position.x);
				 coord_t y=coord_t::truncate
					 (me.y
					  + data(IN_THREAD).current_position.y);

				 grid_map_t::lock grid_lock{lm->grid_map};

				 lm->sliding(IN_THREAD,
					     grid_lock,
					     ref(this),
					     *original_sizes,
					     grabbed_x,
					     grabbed_y,
					     x, y);
			 });
	}
}

LIBCXXW_NAMESPACE_END
