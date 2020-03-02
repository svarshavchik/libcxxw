/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef corner_borderobj_h
#define corner_borderobj_h

#include "corner_borderfwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! A horizontal or a vertical border.

//! \see corner_border

class LIBCXX_HIDDEN corner_borderObj: public elementObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	corner_borderObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~corner_borderObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
