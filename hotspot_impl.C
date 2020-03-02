/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"
#include "x/w/impl/child_element.H"
#include "screen.H"
#include "busy.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/busy.H"
#include "x/w/impl/focus/focusable_element.H"
#include "generic_window_handler.H"
#include "run_as.H"
#include "catch_exceptions.H"

#include <x/mcguffinunordered_multimap.H>

LIBCXXW_NAMESPACE_START

hotspotObj::implObj::implObj()
	: hotspot_temperature_thread_only(temperature::cold)
{
}

hotspotObj::implObj::~implObj()=default;

void hotspotObj::implObj::keyboard_focus(ONLY IN_THREAD,
					 const callback_trigger_t &trigger)
{
	if (!get_hotspot_element().current_keyboard_focus(IN_THREAD))
		is_key_down=false;

	update(IN_THREAD, trigger);
}

void hotspotObj::implObj::pointer_focus(ONLY IN_THREAD,
					const callback_trigger_t &trigger)
{
	if (!get_hotspot_element().current_pointer_focus(IN_THREAD))
		is_button1_down=false;

	update(IN_THREAD, trigger);
}

void hotspotObj::implObj::window_focus_change(ONLY IN_THREAD, bool flag)
{
	if (!flag)
	{
		is_key_down=false;
		update(IN_THREAD, {});
	}
}

bool hotspotObj::implObj::process_key_event(ONLY IN_THREAD, const key_event &ke)
{
	if (activate_on_key(ke))
	{
		bool potential_activation=ke.keypress != is_key_down;

		is_key_down=ke.keypress;
		update(IN_THREAD, &ke);

		if (potential_activation &&
		    get_hotspot_element().activate_for(ke))
			activated(IN_THREAD, &ke);
		return true;
	}
	return false;
}

void hotspotObj::implObj::grabbed_key_event(ONLY IN_THREAD)
{
	is_key_down=false;
	update(IN_THREAD, {});
}

bool hotspotObj::implObj::activate_on_key(const key_event &ke)
{
	return select_key(ke);
}

bool hotspotObj::implObj::process_button_event(ONLY IN_THREAD,
					       const button_event &be,
					       xcb_timestamp_t timestamp)
{
	if (be.button == 1)
	{
		if (is_button1_down == be.press)
			return true; // Could be due to enter/leave. Ignore.

		// This element should have pointer focus. It's possible
		// we get a button event without the pointer focus. This
		// happens if a label_for this focusable element was clicked
		// on, and the button event gets forwarded here.
		//
		// We'll still check activate_for() and do activated(),
		// but do not change the temperature of the hotspot. If the
		// button click opens a popup that grabs pointer focus we
		// have no means of knowing when the button is released.

		if (get_hotspot_element().current_pointer_focus(IN_THREAD))
		{
			is_button1_down=be.press;
			update(IN_THREAD, &be);
		}

		if (get_hotspot_element().activate_for(be))
			activated(IN_THREAD, &be);
		return true;
	}

	return false;
}

void hotspotObj::implObj::update(ONLY IN_THREAD,
				 const callback_trigger_t &trigger)
{
	temperature new_temperature=temperature::cold;

	auto &e=get_hotspot_element();

	if (e.enabled(IN_THREAD) &&
	    (e.current_keyboard_focus(IN_THREAD) ||
	     e.current_pointer_focus(IN_THREAD)))
		new_temperature=temperature::warm;

	if (new_temperature == temperature::warm &&
	    (is_key_down || is_button1_down))
		new_temperature=temperature::hot;

	if (new_temperature == hotspot_temperature(IN_THREAD))
		return;

	hotspot_temperature(IN_THREAD)=new_temperature;
	temperature_changed(IN_THREAD, trigger);
}

void hotspotObj::implObj::temperature_changed(ONLY IN_THREAD,
					      const callback_trigger_t &trigger)
{
}

void hotspotObj::implObj::on_activate(const hotspot_callback_t &new_callback)
{
	get_hotspot_element().THREAD
		->run_as([me=ref<implObj>(this), new_callback]
			 (ONLY IN_THREAD)
			 {
				 me->on_activate(IN_THREAD, new_callback);
			 });
}

void hotspotObj::implObj::on_activate(ONLY IN_THREAD,
				      const hotspot_callback_t &new_callback)
{
	callback(IN_THREAD)=new_callback;
}

void hotspotObj::implObj::activated(ONLY IN_THREAD,
				    const callback_trigger_t &trigger)
{
	if (!callback(IN_THREAD))
		return;

	auto &e=get_hotspot_element();

	try {
		callback(IN_THREAD)(IN_THREAD, trigger, busy_impl{e});
	} REPORT_EXCEPTIONS(&e);
}

bool hotspotObj::implObj::enabled(ONLY IN_THREAD)
{
	return get_hotspot_focusable().focusable_enabled(IN_THREAD);
}

void hotspotObj::implObj::set_shortcut(const shortcut &sc)
{
	if (sc)
		install_shortcut(sc, activated_in_thread(this));
	else
		uninstall_shortcut();
}

LIBCXXW_NAMESPACE_END
