/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainerobj_h
#define listcontainerobj_h

#include "listlayoutmanager/listcontainerfwd.H"
#include "x/w/containerobj.H"
#include "x/w/listlayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

//! An object for a container with a listlayoutmanager

//! \see listlayoutmanager
class LIBCXX_HIDDEN listcontainerObj : public containerObj {
 public:

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	listcontainerObj(const ref<implObj> &impl,
			 const ref<listlayoutmanagerObj::implObj> &list_impl);

	//! Destructor
	~listcontainerObj();
};

LIBCXXW_NAMESPACE_END

#endif
