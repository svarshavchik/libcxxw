/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_menubarlayoutmanagerobj_h
#define x_w_menubarlayoutmanagerobj_h

#include <x/w/menubarlayoutmanagerfwd.H>
#include <x/w/gridlayoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menubarlayoutmanager

class menubarlayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	menubarlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~menubarlayoutmanagerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
