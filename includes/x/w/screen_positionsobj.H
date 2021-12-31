/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positionsobj_h
#define x_w_screen_positionsobj_h

#include <x/w/screen_positionsfwd.H>
#include <vector>
#include <string>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see screen_positions

class screen_positionsObj: virtual public obj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	class config_handleObj;

	class LIBCXX_HIDDEN widget_typeObj;

	//! Constructor
	screen_positionsObj(const ref<implObj> &);

	//! Destructor
	~screen_positionsObj();
};

LIBCXXW_NAMESPACE_END

#endif
