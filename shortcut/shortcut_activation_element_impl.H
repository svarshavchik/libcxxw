/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef shortcut_activation_element_impl_h
#define shortcut_activation_element_impl_h

#include "activated_in_threadfwd.H"
#include "shortcut/installed_shortcutfwd.H"
#include "x/w/generic_windowobj.H"
#include "x/w/shortcut.H"

LIBCXXW_NAMESPACE_START

//! Active shortcut mixin.

//! Handles installation and uninstallation of shortcuts. Must be used
//! with the shortcut_activation_elementObj template. Alternatively,
//! derive from independent_shortcut_activationObj.

class shortcut_activation_element_implObj : virtual public obj {

	//! Currently installed shortcut
	installed_shortcutptr current_shortcut;

	//! Iterator in the window handler where the shortcut is installed.
	shortcut_lookup_t::iterator current_shortcut_iter;

public:

	//! Constructor
	shortcut_activation_element_implObj();

	//! Destructor
	~shortcut_activation_element_implObj();

	//! Return my window handler.

	//! Implemented by the shortcut_activation_elementObj template.

	virtual generic_windowObj::handlerObj &shortcut_window_handler()=0;

	//! Install a new shortcut here.
	void install_shortcut(const shortcut &new_shortcut,
			      const activated_in_thread &what_to_activate);

	//! Uninstall a new shortcut here.
	void uninstall_shortcut();
};

LIBCXXW_NAMESPACE_END

#endif
