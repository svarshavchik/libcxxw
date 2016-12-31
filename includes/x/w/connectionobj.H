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
	~connectionObj() noexcept LIBCXX_HIDDEN;

	//! Numbers of screens on this display.
	size_t screens() const;

	//! Which screen is the default one.
	size_t default_screen() const;

	//! Returns the mcguffin for the underlying display server connection.
	ref<obj> mcguffin() const;
};

LIBCXXW_NAMESPACE_END

#endif
