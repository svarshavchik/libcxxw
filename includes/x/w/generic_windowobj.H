/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_generic_windowobj_h
#define x_w_generic_windowobj_h

#include <x/w/generic_windowfwd.H>
#include <x/w/new_layoutmanagerfwd.H>
#include <x/w/containerobj.H>
#include <x/w/drawableobj.H>
#include <x/obj.H>
#include <x/logger.H>

#include <experimental/string_view>

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
			  const new_layoutmanager &layout_factory)
		LIBCXX_HIDDEN;

	//! Destructor
	~generic_windowObj() LIBCXX_HIDDEN;

	using drawableObj::get_screen;

	//! Set the window's title
	void set_window_title(const std::experimental::string_view &);

	//! Set the window's title
	void set_window_title(const std::experimental::u32string_view &);

	//! Invoke this lambda if the server disconnects unexpectedly.

	//! \see connection
	void on_disconnect(const std::function<void ()> &callback);
};

LIBCXXW_NAMESPACE_END

#endif
