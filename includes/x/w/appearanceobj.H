/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_appearanceobj_h
#define x_w_appearanceobj_h

#include <x/w/appearancefwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see appearance

class appearanceObj: virtual public obj {

public:
	//! Constructor
	appearanceObj();

	//! Destructor
	~appearanceObj();
};

LIBCXXW_NAMESPACE_END

#endif
