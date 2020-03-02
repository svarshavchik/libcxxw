/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "shared_handler_data.H"
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
		   const activated_in_thread &what_to_activate,
		   bool new_global)
{
	uninstall_shortcut();

	auto is=installed_shortcut::create(new_shortcut, what_to_activate);

	auto &h=shortcut_window_handler();

	// Carefully install the shortcut into the right list.
	mpobj<shortcut_lookup_t>::lock
		lock{new_global ? h.handler_data->global_shortcuts
			: h.local_shortcuts};

	current_shortcut_iter=lock->insert({unicode_lc(new_shortcut.unicode),
				is});
	current_shortcut=is;
	global=new_global;
}

void shortcut_activation_element_implObj::uninstall_shortcut()
{
	if (!current_shortcut)
		return;

	// Carefully uninstall where the shortcut was installed.
	auto &h=shortcut_window_handler();
	mpobj<shortcut_lookup_t>::lock
		lock{global ? h.handler_data->global_shortcuts
			: h.local_shortcuts};

	lock->erase(current_shortcut_iter);
	current_shortcut=nullptr;
}


LIBCXXW_NAMESPACE_END
