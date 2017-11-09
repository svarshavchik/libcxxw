/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "shared_handler_data.H"
#include "popup/popup.H"
#include "generic_window_handler.H"
#include "shortcut/installed_shortcut.H"
#include "x/w/button_event.H"

LIBCXXW_NAMESPACE_START

class shared_handler_dataObj::handler_mcguffinObj : virtual public obj {

public:
	weakptr<ptr<popupObj::handlerObj>> handler;

	handler_mcguffinObj(const ref<popupObj::handlerObj> &handler)
		: handler(handler)
	{
	}

	void hide(IN_THREAD_ONLY)
	{
		auto p=handler.getptr();

		if (!p)
			return;

		p->request_visibility(IN_THREAD, false);
	}
};

shared_handler_dataObj::shared_handler_dataObj()
	: opened_menu_popups(opened_menu_popups_t::create())
{
}

shared_handler_dataObj::~shared_handler_dataObj()=default;

void shared_handler_dataObj::set_toplevel_handler(const ref<generic_windowObj
						  ::handlerObj> &h)
{
	toplevel_handler=h;
}

ref<obj> shared_handler_dataObj
::opening_combobox_popup(IN_THREAD_ONLY,
			 const ref<popupObj::handlerObj> &popup)
{
	auto mcguffin=ref<handler_mcguffinObj>::create(popup);

	close_combobox_popup(IN_THREAD);

	opened_combobox_popup=mcguffin;

	return mcguffin;
}

void shared_handler_dataObj
::closing_combobox_popup(IN_THREAD_ONLY,
			 const popupObj::handlerObj &popup)
{
}

void shared_handler_dataObj::close_combobox_popup(IN_THREAD_ONLY)
{
	auto p=opened_combobox_popup.getptr();

	if (p)
		p->hide(IN_THREAD);
}

void shared_handler_dataObj
::hide_menu_popups_until(IN_THREAD_ONLY,
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
::opening_menu_popup(IN_THREAD_ONLY,
		     const ref<popupObj::handlerObj> &popup)
{
	close_combobox_popup(IN_THREAD);

	auto mcguffin=ref<handler_mcguffinObj>::create(popup);

	auto nesting_level=popup->nesting_level;

	hide_menu_popups_until(IN_THREAD,
			       opened_menu_popups->lower_bound(nesting_level));

	opened_menu_popups->insert(nesting_level, mcguffin);

	return mcguffin;
}

void shared_handler_dataObj
::closing_menu_popup(IN_THREAD_ONLY,
		     const popupObj::handlerObj &popup)
{
	hide_menu_popups_until(IN_THREAD, opened_menu_popups
			       ->lower_bound(popup.nesting_level));
}

void shared_handler_dataObj::close_all_menu_popups(IN_THREAD_ONLY)
{
	hide_menu_popups_until(IN_THREAD, opened_menu_popups->end());
	auto h=toplevel_handler.getptr();
	if (h)
		h->unset_keyboard_focus(IN_THREAD);
}

bool shared_handler_dataObj
::handle_key_event(IN_THREAD_ONLY,
		   const xcb_key_release_event_t *event,
		   bool keypress)
{
	// There are popups open, and we have a key event.

	// If there's a combo-box popup, it either deals with the keypress,
	// or not.
	auto combobox_popup=opened_combobox_popup.getptr();

	if (combobox_popup)
	{
		auto handler=combobox_popup->handler.getptr();

		if (handler && handler->data(IN_THREAD).requested_visibility)
			return handler->handle_key_event(IN_THREAD, event,
							 keypress);
		return false;
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

		if (handler->handle_key_event(IN_THREAD, event, keypress))
			return true;
	}
	return false;
}

void shared_handler_dataObj
::reporting_button_event_to(IN_THREAD_ONLY,
			    const ref<generic_windowObj::handlerObj> &from,
			    const ref<generic_windowObj::handlerObj> &to,
			    const button_event &be)
{
	if (be.press)
		return;

	if (from == to)
		return;

	from->pointer_focus_lost(IN_THREAD);

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

		from->pointer_focus_lost(IN_THREAD);
	}
}

ptr<generic_windowObj::handlerObj>
shared_handler_dataObj::find_popup_for_xy(IN_THREAD_ONLY,
					const motion_event &me)
{
	// If there's a combo-box popup, all motion events go there.
	auto combobox_popup=opened_combobox_popup.getptr();

	if (combobox_popup)
	{
		auto handler=combobox_popup->handler.getptr();

		if (handler && handler->data(IN_THREAD).requested_visibility)
			return handler;
		return ptr<generic_windowObj::handlerObj>();
	}

	// Otherwise go through all the menu popups, and find the first one
	// that's under the pointer.

	for (auto &b:*opened_menu_popups)
	{
		auto popup=b.second.getptr();

		if (!popup)
			continue;

		auto handler=popup->handler.getptr();

		if (!handler || !handler->data(IN_THREAD).requested_visibility)
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

LIBCXXW_NAMESPACE_END
