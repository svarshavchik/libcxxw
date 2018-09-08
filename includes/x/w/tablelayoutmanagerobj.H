/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanagerobj_h
#define x_w_tablelayoutmanagerobj_h

#include <x/w/tablelayoutmanagerfwd.H>
#include <x/w/listlayoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see tablelayoutmanager

class tablelayoutmanagerObj : public listlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Internal implementation object.
	class table_synchronized_axisObj;

	//! Constructor
	tablelayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~tablelayoutmanagerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
