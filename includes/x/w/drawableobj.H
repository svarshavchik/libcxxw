/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_drawableobj_H
#define x_w_drawableobj_H

#include <x/obj.H>
#include <x/w/pictformatfwd.H>

LIBCXXW_NAMESPACE_START

//! A drawable object.

//! \internal

class drawableObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation details.
	const ref<implObj> impl;

	//! Constructor
	drawableObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~drawableObj() LIBCXX_HIDDEN;

	//! Return the drawable's pictformat.

	const_pictformat get_pictformat() const;
};

#if 0
{
	{
#endif
	}
}

#endif
