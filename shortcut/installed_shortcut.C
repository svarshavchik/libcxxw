/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "shortcut/installed_shortcut.H"

LIBCXXW_NAMESPACE_START

installed_shortcutObj
::installed_shortcutObj(const shortcut &installed_shortcut,
			const activated_in_thread &what_to_activate)
	: installed_shortcut_thread_only(installed_shortcut),
	  activate(what_to_activate)
{
}

installed_shortcutObj::~installed_shortcutObj()=default;

void installed_shortcutObj::activated(ONLY IN_THREAD,
				      const callback_trigger_t &trigger)
{
	auto p=activate.getptr();

	if (p)
		p->activated(IN_THREAD, trigger);
}

bool installed_shortcutObj::enabled(ONLY IN_THREAD)
{
	auto p=activate.getptr();

	if (p)
		return p->enabled(IN_THREAD);

	return false;
}

LIBCXXW_NAMESPACE_END
