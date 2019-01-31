/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef straight_borderobj_h
#define straight_borderobj_h

#include "straight_borderfwd.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

//! A horizontal or a vertical border.

//! \see straight_border

class LIBCXX_HIDDEN straight_borderObj: public elementObj {

public:
	class implObj;

	const ref<implObj> impl;

	//! Constructor
	straight_borderObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~straight_borderObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
