/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowobj_h
#define x_w_generic_windowobj_h

#include <x/w/generic_windowfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/containerobj.H>
#include <x/w/drawableobj.H>
#include <x/obj.H>
#include <x/logger.H>

#include <string_view>

LIBCXXW_NAMESPACE_START

//! Generic window facilities.

//! \see generic_window

class generic_windowObj : public containerObj,
			  public drawableObj {

public:
	LOG_CLASS_SCOPE;

	class implObj;
	class handlerObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	generic_windowObj(const ref<implObj> &impl,
			  const ref<layoutmanagerObj::implObj> &lm)
		LIBCXX_HIDDEN;

	//! Destructor
	~generic_windowObj() LIBCXX_HIDDEN;

	using drawableObj::get_screen;

	//! Set the window's title
	void set_window_title(const std::string_view &);

	//! Set the window's title
	void set_window_title(const std::u32string_view &);

	//! Set the window's class
	void set_window_class(const std::string_view &instance,
			      const std::string_view &resource);

	//! Invoke this lambda if the server disconnects unexpectedly.

	//! \see connection
	void on_disconnect(const std::function<void ()> &callback);
};

LIBCXXW_NAMESPACE_END

#endif
