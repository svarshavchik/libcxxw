/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "hotspot.H"
#include "child_element.H"
#include "screen.H"
#include "element_screen.H"
#include "focus/focusable_element.H"
#include "connection_thread.H"
#include "catch_exceptions.H"

LIBCXXW_NAMESPACE_START

static hotspot_callback_t no_callback() { return [] {}; }

hotspotObj::implObj::implObj()
	: hotspot_temperature_thread_only(temperature::cold),
	  callback_thread_only(no_callback)
{
}

hotspotObj::implObj::~implObj()=default;

void hotspotObj::implObj::keyboard_focus(IN_THREAD_ONLY,
					 focus_change event,
					 const ref<elementObj::implObj> &ptr)
{
	update(IN_THREAD);
}

bool hotspotObj::implObj::process_key_event(IN_THREAD_ONLY, char32_t unicode,
					    uint32_t keysym, bool keypress)
{
	if (unicode == ' ' || unicode == '\n')
	{
		bool activated_flag=!keypress && is_key_down;

		is_key_down=keypress;
		update(IN_THREAD);

		if (activated_flag)
			activated(IN_THREAD);
		return true;
	}
	return false;
}

void hotspotObj::implObj::update(IN_THREAD_ONLY)
{
	temperature new_temperature=temperature::cold;

	if (get_hotspot_element().current_keyboard_focus(IN_THREAD))
		new_temperature=temperature::warm;
	else
		is_key_down=false; // No focus, no key.

	if (new_temperature == temperature::warm && is_key_down)
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
	get_hotspot_element().get_screen()->impl->thread
		->run_as(RUN_AS,
			 [me=ref<implObj>(this), new_callback]
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
		callback(IN_THREAD)();
	} CATCH_EXCEPTIONS;
}

LIBCXXW_NAMESPACE_END
