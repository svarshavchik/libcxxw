/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_main_windowobj_h
#define x_w_main_windowobj_h

#include <x/w/main_windowfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/generic_windowobj.H>
#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/obj.H>
#include <x/logger.H>

LIBCXXW_NAMESPACE_START

//! Main application window.

//! \see main_window

class main_windowObj : public generic_windowObj {

public:
	LOG_CLASS_SCOPE;

	class implObj;
	class handlerObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	main_windowObj(const ref<implObj> &impl,
		       const ref<layoutmanagerObj::implObj> &lm)
		LIBCXX_HIDDEN;

	//! Destructor
	~main_windowObj() LIBCXX_HIDDEN;

	//! Return the layout manager for the menu bar.

	menubarlayoutmanager get_menubarlayoutmanager();

	//! Return the layout manager for the menu bar.

	const_menubarlayoutmanager get_menubarlayoutmanager() const;

	//! Return the menu bar itself.

	container get_menubar();

	//! Return the menu bar itself.
	const_container get_menubar() const;

	//! Install a callback that gets invoke when the window close button is clicked.

	void on_delete(const std::function<void ()> &callback);

private:

	//! Inherited from containerObj

	ref<layoutmanagerObj::implObj> get_layout_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
