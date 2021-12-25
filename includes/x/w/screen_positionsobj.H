/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_screen_positionsobj_h
#define x_w_screen_positionsobj_h

#include <x/w/screen_positionsfwd.H>
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

	//! Constructor
	screen_positionsObj();

	//! Destructor
	~screen_positionsObj();

	//! Constructor

	//! Construct a new instance and initialize it with the contents
	//! of the specified file.
	screen_positionsObj(const std::string &filename);
};

LIBCXXW_NAMESPACE_END

#endif
