/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "shortcut/shortcut_activation_element_impl.H"
#include "activated_in_thread.H"
#include "shortcut/installed_shortcut.H"
#include "generic_window_handler.H"
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

shortcut_activation_element_implObj::shortcut_activation_element_implObj()
=default;

shortcut_activation_element_implObj::~shortcut_activation_element_implObj()
=default;

void shortcut_activation_element_implObj
::install_shortcut(const shortcut &new_shortcut,
		   const activated_in_thread &what_to_activate)
{
	uninstall_shortcut();

	auto is=installed_shortcut::create(new_shortcut, what_to_activate);

	mpobj<shortcut_lookup_t>::lock
		lock{shortcut_window_handler().installed_shortcuts};

	current_shortcut_iter=lock->insert({unicode_lc(new_shortcut.unicode),
				is});
	current_shortcut=is;
}

void shortcut_activation_element_implObj::uninstall_shortcut()
{
	if (!current_shortcut)
		return;

	mpobj<shortcut_lookup_t>::lock
		lock{shortcut_window_handler().installed_shortcuts};

	lock->erase(current_shortcut_iter);
	current_shortcut=nullptr;
}


LIBCXXW_NAMESPACE_END
