/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "shared_handler_data.H"
#include "popup/popup.H"
#include "generic_window_handler.H"
#include "shortcut/installed_shortcut.H"
#include "x/w/button_event.H"
#include "x/w/impl/focus/focusable.H"
#include "x/w/impl/child_element.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN shared_handler_dataObj::handler_mcguffinObj
	: virtual public obj {

public:
	weakptr<ptr<popupObj::handlerObj>> handler;

	handler_mcguffinObj(const ref<popupObj::handlerObj> &handler)
		: handler(handler)
	{
	}

	void hide(ONLY IN_THREAD)
	{
		auto p=handler.getptr();

		if (!p)
			return;

		p->request_visibility(IN_THREAD, false);
	}
};

shared_handler_dataObj::shared_handler_dataObj()
	: opened_exclusive_popups{opened_exclusive_popups_t::create()},
	  opened_menu_popups(opened_menu_popups_t::create())
{
}

shared_handler_dataObj::~shared_handler_dataObj()=default;

void shared_handler_dataObj::set_toplevel_handler(const ref<generic_windowObj
						  ::handlerObj> &h)
{
	toplevel_handler=h;
}

ref<obj> shared_handler_dataObj
::opening_exclusive_popup(ONLY IN_THREAD,
			 const ref<popupObj::handlerObj> &popup)
{
	auto mcguffin=ref<handler_mcguffinObj>::create(popup);

	opened_exclusive_popups->push_front(mcguffin);

	return mcguffin;
}

void shared_handler_dataObj
::closing_exclusive_popup(ONLY IN_THREAD,
			 const popupObj::handlerObj &popup)
{
}

void shared_handler_dataObj::close_all_exclusive_popups(ONLY IN_THREAD)
{
	for (const auto &p:*opened_exclusive_popups)
	{
		auto pp=p.getptr();

		if (pp)
			pp->hide(IN_THREAD);
	}
}

void shared_handler_dataObj
::hide_menu_popups_until(ONLY IN_THREAD,
			 opened_menu_popups_t::base::iterator iter)
{
	for (auto b=opened_menu_popups->begin(); b != iter; ++b)
	{
		auto p=b->second.getptr();

		if (p)
			p->hide(IN_THREAD);
	}
}

ref<obj> shared_handler_dataObj
::opening_menu_popup(ONLY IN_THREAD,
		     const ref<popupObj::handlerObj> &popup)
{
	close_all_exclusive_popups(IN_THREAD);

	auto mcguffin=ref<handler_mcguffinObj>::create(popup);

	auto nesting_level=popup->nesting_level;

	hide_menu_popups_until(IN_THREAD,
			       opened_menu_popups->lower_bound(nesting_level));

	opened_menu_popups->insert(nesting_level, mcguffin);

	return mcguffin;
}

void shared_handler_dataObj
::closing_menu_popup(ONLY IN_THREAD,
		     const popupObj::handlerObj &popup)
{
	hide_menu_popups_until(IN_THREAD, opened_menu_popups
			       ->lower_bound(popup.nesting_level));
}

void shared_handler_dataObj::close_all_menu_popups(ONLY IN_THREAD)
{
	hide_menu_popups_until(IN_THREAD, opened_menu_popups->end());
#if 0
	// invoked on_validate callback in an input field, if it had
	// keyboard focus.
	auto h=toplevel_handler.getptr();
	if (h)
		h->unset_keyboard_focus(IN_THREAD, {});
#endif
}

bool shared_handler_dataObj
::handle_key_event(ONLY IN_THREAD,
		   const ref<generic_windowObj::handlerObj> &key_event_from,
		   const xcb_key_release_event_t *event,
		   uint16_t sequencehi,
		   bool keypress)
{
	// There are popups open, and we have a key event.

	// If there's a combo-box popup, it either deals with the keypress,
	// or not.

	for (auto &b:*opened_exclusive_popups)
	{
		auto exclusive_popup=b.getptr();

		if (!exclusive_popup)
			continue;

		auto handler=exclusive_popup->handler.getptr();

		if (handler &&
		    handler->popup_accepts_key_events(IN_THREAD))
		{
			// Once we found an exclusive popup it will
			// process the keypress. Even if it doesn't, we
			// consider it processed.
			//
			// The input focus in the main window is on a button
			// that opened an attached popup. In the attached popup
			// the cursor is in an input field. The input field
			// ignores the cursor left, and we don't want to
			// return false, and have the main window process this
			// key event and have the popup button close the
			// popup.
			reporting_key_event_to(IN_THREAD, key_event_from,
					       handler,
					       event, keypress);
			(void)handler->forward_key_event_to_xim(IN_THREAD,
								event,
								sequencehi,
								keypress);
			return true;
		}
	}

	// Check for visible menu popups.

	for (auto b=opened_menu_popups->begin(),
		     e=opened_menu_popups->end();
	     b != e; ++b)
	{
		auto p=b->second.getptr();

		if (!p)
			continue;

		auto handler=p->handler.getptr();

		if (!handler || !handler->data(IN_THREAD).requested_visibility)
			continue;

		reporting_key_event_to(IN_THREAD, key_event_from, handler,
				       event, keypress);
		(void)handler->forward_key_event_to_xim(IN_THREAD, event,
							sequencehi,
							keypress);
		return true;
	}
	return false;
}

void shared_handler_dataObj
::reporting_pointer_xy_to(ONLY IN_THREAD,
			  const ref<generic_windowObj::handlerObj> &from,
			  const ref<generic_windowObj::handlerObj> &to,
			  const callback_trigger_t &trigger)
{
	if (from != to)
		from->pointer_focus_lost(IN_THREAD, trigger);

	for (auto &b:*opened_menu_popups)
	{
		auto popup=b.second.getptr();

		if (!popup)
			continue;

		ptr<generic_windowObj::handlerObj>
			handler=popup->handler.getptr();

		if (!handler)
			continue;

		if (handler == to)
			continue;

		handler->pointer_focus_lost(IN_THREAD, trigger);
	}

	for (auto &b:*opened_exclusive_popups)
	{
		auto popup=b.getptr();

		if (!popup)
			continue;

		ptr<generic_windowObj::handlerObj>
			handler=popup->handler.getptr();

		if (!handler)
			continue;

		if (handler == to)
			continue;

		handler->pointer_focus_lost(IN_THREAD, trigger);
	}
}

void shared_handler_dataObj
::reporting_key_event_to(ONLY IN_THREAD,
			 const ref<generic_windowObj::handlerObj> &from,
			 const ref<generic_windowObj::handlerObj> &to,
			 const xcb_key_release_event_t *event,
			 bool keypress)
{
	if (keypress)
		return;

	if (from == to)
		return;

	{
		auto &from_kb=from->most_recent_keyboard_focus(IN_THREAD);

		if (from_kb)
			from_kb->get_focusable_element()
				.grabbed_key_event(IN_THREAD);
	}

	for (auto &b:*opened_menu_popups)
	{
		auto popup=b.second.getptr();

		if (!popup)
			continue;

		ptr<generic_windowObj::handlerObj>
			handler=popup->handler.getptr();

		if (!handler)
			continue;

		if (handler == to)
			continue;

		auto &kb=handler->most_recent_keyboard_focus(IN_THREAD);

		if (kb)
			kb->get_focusable_element()
				.grabbed_key_event(IN_THREAD);
	}

	for (auto &b:*opened_exclusive_popups)
	{
		auto popup=b.getptr();

		if (!popup)
			continue;

		ptr<generic_windowObj::handlerObj>
			handler=popup->handler.getptr();

		if (!handler)
			continue;

		if (handler == to)
			continue;

		auto &kb=handler->most_recent_keyboard_focus(IN_THREAD);

		if (kb)
			kb->get_focusable_element()
				.grabbed_key_event(IN_THREAD);
	}
}

ptr<generic_windowObj::handlerObj>
shared_handler_dataObj::find_popup_for_xy(ONLY IN_THREAD,
					const motion_event &me)
{
	// If there's a combo-box popup, all motion events go there.

	bool have_exclusive_popup=false;

	for (auto &b:*opened_exclusive_popups)
	{
		auto exclusive_popup=b.getptr();

		if (!exclusive_popup)
			continue;

		auto handler=exclusive_popup->handler.getptr();

		if (handler && handler->popup_accepts_button_events(IN_THREAD))
			return handler;
		have_exclusive_popup=true;
	}

	if (have_exclusive_popup)
		return ptr<generic_windowObj::handlerObj>();

	// Otherwise go through all the menu popups, and find the first one
	// that's under the pointer.

	for (auto &b : *opened_menu_popups)
	{
		auto popup=b.second.getptr();

		if (!popup)
			continue;

		auto handler=popup->handler.getptr();

		if (!handler ||
		    !handler->popup_accepts_button_events(IN_THREAD))
			continue;

		auto &h=*handler;

		if (me.x < h.root_x(IN_THREAD) ||
		    me.y < h.root_y(IN_THREAD))
			continue;

		dim_t x=dim_t::truncate(me.x-h.root_x(IN_THREAD));
		dim_t y=dim_t::truncate(me.y-h.root_y(IN_THREAD));

		if (x >= h.get_width() || y >= h.get_height())
			continue;

		return handler;
	}
	return ptr<generic_windowObj::handlerObj>();
}

void shared_handler_dataObj::opening_dialog(ONLY IN_THREAD)
{
	close_all_exclusive_popups(IN_THREAD);
	hide_menu_popups_until(IN_THREAD, opened_menu_popups->end());
}

void shared_handler_dataObj::closing_dialog(ONLY IN_THREAD)
{
}

LIBCXXW_NAMESPACE_END
