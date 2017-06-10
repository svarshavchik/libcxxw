/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemlayoutmanagerobj_h
#define listitemlayoutmanagerobj_h

#include "x/w/layoutmanagerobj.H"
#include "listlayoutmanager/listitemlayoutmanagerfwd.H"

LIBCXXW_NAMESPACE_START

//! A layout manager for a cell in a list/grid layout manager.

//! \see listitemlayoutmanager

class LIBCXX_HIDDEN listitemlayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listitemlayoutmanagerObj(const ref<implObj> &impl);

	//! Destructor
	~listitemlayoutmanagerObj();

};

LIBCXXW_NAMESPACE_END

#endif
