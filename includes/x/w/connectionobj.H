/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_connectionobj_h
#define x_w_connectionobj_h

#include <x/w/connectionfwd.H>
#include <x/w/screenfwd.H>
#include <x/obj.H>
#include <vector>
#include <experimental/string_view>

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
	connectionObj(const std::experimental::string_view &display=
		      std::experimental::string_view());

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

	std::pair<std::string, int> current_theme();
};

LIBCXXW_NAMESPACE_END

#endif
