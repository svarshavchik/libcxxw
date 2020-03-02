/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagetabgridlayoutmanagerobj_h
#define x_w_pagetabgridlayoutmanagerobj_h

#include "x/w/pagetabgridlayoutmanagerfwd.H"
#include "x/w/gridlayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pagetabgridlayoutmanager

class LIBCXX_HIDDEN pagetabgridlayoutmanagerObj : public gridlayoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pagetabgridlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~pagetabgridlayoutmanagerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
