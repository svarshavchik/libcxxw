/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef main_window_h
#define main_window_h

#include "x/w/main_window.H"
#include "x/w/containerfwd.H"
#include "generic_window.H"
#include "xim_generic_window.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN main_windowObj::implObj
	: public generic_windowObj::implObj,
	  public xim_generic_windowObj {

 public:

	//! Constructor
	implObj(const ref<handlerObj> &handler,
		const container &menu_and_app_container,
		const container &menubar_container,
		const container &app_container);

	//! Destructor
	~implObj();

	//! The handler for the connection thread events.
	const ref<handlerObj> handler;

	//! The container installed in the main window's peephole

	//! The main window's peephole has a container with two elements,
	//! a menu bar, which is a container with the menu bar layout manager,
	//! and the app_container.

	const container menu_and_app_container;

	//! The container with the menubar.

	const container menubar_container;

	//! The container with the main window's specified layout manager.

	//! The main window is the real top level container, but the requested
	//! layout manager gets created for this container.
	//!
	//! main_window overrides get_layout_impl() and returns the
	//! app_container's layout manager, making it look like it's
	//! the layout manager for the top level main_window, but it's really
	//! the layout manager for the app_container.

	const container app_container;

	//! Install a callback to invoke upon close.

	void on_delete(const std::function<void ()> &callback);
};

LIBCXXW_NAMESPACE_END

#endif
