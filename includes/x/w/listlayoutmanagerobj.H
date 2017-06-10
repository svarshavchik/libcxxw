/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerobj_h
#define x_w_listlayoutmanagerobj_h

#include <x/w/listlayoutmanagerfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/factoryfwd.H>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see listlayoutmanager

class listlayoutmanagerObj : public gridlayoutmanagerObj {

public:
	friend class gridfactoryObj;

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~listlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append a new list item.
	factory append_item();
};

LIBCXXW_NAMESPACE_END

#endif
