/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_containerobj_h
#define x_w_focusable_containerobj_h

#include <x/w/focusable_containerfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>

LIBCXXW_NAMESPACE_START

//! A focusable widget that's implemented as an internal container.

//! \see focusable_container

class focusable_containerObj : public containerObj, public focusableObj {

public:
	using containerObj::containerObj;

	//! Destructor

	~focusable_containerObj();
};

LIBCXXW_NAMESPACE_END

#endif
