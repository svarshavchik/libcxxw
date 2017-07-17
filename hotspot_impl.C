/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"
#include "child_element.H"
#include "screen.H"
#include "busy.H"
#include "element_screen.H"
#include "x/w/key_event.H"
#include "x/w/button_event.H"
#include "x/w/busy.H"
#include "focus/focusable_element.H"
#include "generic_window_handler.H"
#include "run_as.H"
#include "catch_exceptions.H"

#include <x/mcguffinunordered_multimap.H>

LIBCXXW_NAMESPACE_START

static void no_callback(const busy &ignore)
{
}

hotspotObj::implObj::implObj()
	: hotspot_temperature_thread_only(temperature::cold),
	  callback_thread_only(no_callback)
{
}

hotspotObj::implObj::~implObj()=default;

void hotspotObj::implObj::hotspot_deinitialize(IN_THREAD_ONLY)
{
	set_shortcut(IN_THREAD, shortcut());
}

void hotspotObj::implObj::keyboard_focus(IN_THREAD_ONLY)
{
	if (!get_hotspot_element().current_keyboard_focus(IN_THREAD))
		is_key_down=false;

	update(IN_THREAD);
}

void hotspotObj::implObj::pointer_focus(IN_THREAD_ONLY)
{
	if (!get_hotspot_element().current_pointer_focus(IN_THREAD))
		is_button1_down=false;

	update(IN_THREAD);
}

bool hotspotObj::implObj::process_key_event(IN_THREAD_ONLY, const key_event &ke)
{
	if (ke.notspecial() && activate_on_key(ke))
	{
		bool activated_flag=!ke.keypress && is_key_down;

		is_key_down=ke.keypress;
		update(IN_THREAD);

		if (activated_flag)
			activated(IN_THREAD);
		return true;
	}
	return false;
}

bool hotspotObj::implObj::activate_on_key(const key_event &ke)
{
	return (ke.unicode == ' ' || ke.unicode == '\n');
}

bool hotspotObj::implObj::process_button_event(IN_THREAD_ONLY,
					       const button_event &be,
					       xcb_timestamp_t timestamp)
{
	if (be.button == 1)
	{
		if (is_button1_down == be.press)
			return true; // Could be due to enter/leave. Ignore.

		is_button1_down=be.press;
		update(IN_THREAD);

		if (!be.press)
			activated(IN_THREAD);
		return true;
	}

	return false;
}

void hotspotObj::implObj::update(IN_THREAD_ONLY)
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
	temperature_changed(IN_THREAD);
}

void hotspotObj::implObj::temperature_changed(IN_THREAD_ONLY)
{
}

void hotspotObj::implObj::on_activate(const hotspot_callback_t &new_callback)
{
	get_hotspot_element().THREAD
		->run_as([me=ref<implObj>(this), new_callback]
			 (IN_THREAD_ONLY)
			 {
				 me->on_activate(IN_THREAD, new_callback);
			 });
}

void hotspotObj::implObj::on_activate(IN_THREAD_ONLY,
				      const hotspot_callback_t &new_callback)
{
	callback(IN_THREAD)=new_callback;
}

LOG_FUNC_SCOPE_DECL(LIBCXXW_NAMESPACE::hotspot, hotspot_log);

void hotspotObj::implObj::activated(IN_THREAD_ONLY)
{
	LOG_FUNC_SCOPE(hotspot_log);

	try {
		callback(IN_THREAD)(busy_impl{get_hotspot_element(),
					IN_THREAD});
	} CATCH_EXCEPTIONS;
}

void hotspotObj::implObj::set_shortcut(IN_THREAD_ONLY, const shortcut &sc)
{
	auto &h=get_hotspot_element().get_window_handler();

	if (hotspot_shortcut) // Existing shortcut
		h.shortcut_lookup(IN_THREAD).erase(installed_hotspot_iter);

	if (sc)
	{
		installed_hotspot_iter=h.shortcut_lookup(IN_THREAD)
			.insert({sc.unicode, ref<implObj>(this)});
	}

	hotspot_shortcut=sc;
}

LIBCXXW_NAMESPACE_END
