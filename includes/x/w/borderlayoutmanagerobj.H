/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_borderlayoutmanagerobj_h
#define x_w_borderlayoutmanagerobj_h

#include <x/w/borderlayoutmanagerfwd.H>
#include <x/w/singletonlayoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see borderlayoutmanager

class borderlayoutmanagerObj : public singletonlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	borderlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~borderlayoutmanagerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
