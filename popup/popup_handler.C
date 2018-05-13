/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_impl.H"
#include "popup/popup_handler.H"
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

LIBCXXW_NAMESPACE_START

popupObj::handlerObj::handlerObj(ONLY IN_THREAD,
				 const ref<generic_windowObj::handlerObj>
				 &parent,
				 const color_arg &background_color,
				 size_t nesting_level)
	: superclass_t(IN_THREAD, parent->get_screen(),
		       background_color,
		       parent->handler_data,
		       nesting_level),
	popup_parent(parent)
{
	update_user_time(IN_THREAD);
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

void popupObj::handlerObj::frame_extents_updated(ONLY IN_THREAD)
{
	set_popup_position(IN_THREAD);
}

void popupObj::handlerObj::horizvert_updated(ONLY IN_THREAD)
{
	set_popup_position(IN_THREAD);
}

void popupObj::handlerObj::theme_updated(ONLY IN_THREAD,
					 const defaulttheme &new_theme)
{
	generic_windowObj::handlerObj::theme_updated(IN_THREAD, new_theme);

	set_popup_position(IN_THREAD);
}

void popupObj::handlerObj::set_popup_position(ONLY IN_THREAD)
{
	auto hv=get_horizvert(IN_THREAD);

	rectangle r=*mpobj<rectangle>::lock{current_position};

	// Opening bid: our preferred size.

	r.width=hv->horiz.preferred();
	r.height=hv->vert.preferred();

	auto screen_width=screenref->impl->width_in_pixels();
	auto screen_height=screenref->impl->height_in_pixels();

	recalculate_popup_position(IN_THREAD,
				   r,
				   screen_width,
				   screen_height);

	// Whatever recalculate_popup_position() wanted, adjust the popup
	// so that it fits on the screen.

	if (r.width > screen_width)
		r.width=screen_width;

	if (r.height > screen_height)
		r.height=screen_height;

	coord_t max_x=coord_t::truncate(screen_width-r.width);
	coord_t max_y=coord_t::truncate(screen_height-r.height);

	if (r.x < 0)
		r.x=0;
	if (r.y < 0)
		r.y=0;

	if (r.x > max_x)
		r.x=max_x;
	if (r.y > max_y)
		r.y=max_y;

	{
		mpobj<rectangle>::lock lock{current_position};

		if (r == *lock)
			return; // No change
	}

	auto w=r.width;
	auto h=r.height;

	// ConfigureWindow() does not like width and height of 0.
	if (w == 0 || h == 0)
		w=h=1;

	values_and_mask configure_window_vals
		(XCB_CONFIG_WINDOW_WIDTH,
		 (dim_t::value_type)w,

		 XCB_CONFIG_WINDOW_HEIGHT,
		 (dim_t::value_type)h,

		 XCB_CONFIG_WINDOW_X,
		 (coord_t::value_type)r.x,

		 XCB_CONFIG_WINDOW_Y,
		 (coord_t::value_type)r.y);

	xcb_configure_window(IN_THREAD->info->conn, id(),
			     configure_window_vals.mask(),
			     configure_window_vals.values().data());

#ifdef POPUP_SIZE_SET
	POPUP_SIZE_SET();
#endif

	// Do not wait for the ConfigureNotify event, take the bull by the
	// horns. When it arrives it'll be ignored.

	generic_windowObj::handlerObj::configure_notify_received(IN_THREAD, r);
	generic_windowObj::handlerObj::process_configure_notify(IN_THREAD, r);
}

void popupObj::handlerObj::process_configure_notify(ONLY IN_THREAD,
						    const rectangle &)
{
	// Ignoring the ConfigureNotify event, see?
}

void popupObj::handlerObj::configure_notify_received(ONLY IN_THREAD,
						     const rectangle &)
{
	// Ignoring the ConfigureNotify event, see?
}

void popupObj::handlerObj::absolute_location_updated(ONLY IN_THREAD)
{
	superclass_t::absolute_location_updated(IN_THREAD);

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

void popupObj::handlerObj::set_inherited_visibility(ONLY IN_THREAD,
						    inherited_visibility_info
						    &visibility_info)
{

	if (visibility_info.flag)
	{
		popup_opened(IN_THREAD);
		opened_mcguffin=get_opened_mcguffin(IN_THREAD);
	}

	generic_windowObj::handlerObj::set_inherited_visibility
		(IN_THREAD, visibility_info);

	if (!visibility_info.flag)
	{
		opened_mcguffin=nullptr;
		released_opened_mcguffin(IN_THREAD);
		closing_popup(IN_THREAD);
		unset_keyboard_focus(IN_THREAD, {});
	}
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


LIBCXXW_NAMESPACE_END
