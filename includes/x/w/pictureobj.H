/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pictureobj_h
#define x_w_pictureobj_h

#include <x/w/namespace.H>
#include <x/w/pictformatfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A picture object.

//! \see picture

class pictureObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details

	const ref<implObj> impl;

	//! Constructor

	pictureObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor

	~pictureObj() noexcept;
};

LIBCXXW_NAMESPACE_END

#endif
