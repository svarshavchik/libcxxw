/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_fontobjectsetobj_H
#define x_w_fontobjectsetobj_H

#include <x/ref.H>
#include <x/obj.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! A facade for the fontconfig library's internal FcObjectset object.

class LIBCXX_HIDDEN objectsetObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	objectsetObj(const ref<implObj> &implArg);

	//! Destructor
	~objectsetObj();

	//! Add a string to an object set.

	void add(const std::string_view &s);
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
