/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connectionobj_h
#define x_w_connectionobj_h

#include <x/w/connectionfwd.H>
#include <x/w/screenfwd.H>
#include <x/w/pictformatfwd.H>
#include <x/obj.H>
#include <vector>
#include <string_view>

#include <functional>

LIBCXXW_NAMESPACE_START

//! A connection to the server

//! \see connection

class connectionObj : virtual public obj {

 public:
	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor
	connectionObj(const std::string_view &display=
		      std::string_view());

	//! Destructor
	~connectionObj() LIBCXX_HIDDEN;

	//! Numbers of screens on this display.
	size_t screens() const;

	//! Which screen is the default one.
	size_t default_screen() const;

	//! Returns the mcguffin for the underlying display server connection.
	ref<obj> mcguffin() const;

	//! Invoke this lambda if the server disconnects unexpectedly.

	void on_disconnect(const std::function<void ()> &callback);

	//! Returns the current theme and scaling factor.

	std::pair<std::string, int> current_theme() const;

	//! Sets a new theme and scaling factor.

	void set_theme(const std::string &identifier,
		       int factor,
		       bool this_connection_only);

	//! Sets and saves new theme and scaling factor.

	void set_and_save_theme(const std::string &identifier,
				int factor);

	//! Shortcut to render information.
	const_pictformat find_alpha_pictformat_by_depth(depth_t d) const;
};

LIBCXXW_NAMESPACE_END

#endif
