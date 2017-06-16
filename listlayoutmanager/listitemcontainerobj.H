/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemcontainerobj_h
#define listitemcontainerobj_h

#include <x/w/containerobj.H>
#include "listlayoutmanager/listitemcontainerfwd.H"
#include "listlayoutmanager/listitemlayoutmanagerobj.H"
#include <x/obj.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! List item container object.

//! \see listitemcontainer

class LIBCXX_HIDDEN listitemcontainerObj : public containerObj {

 public:

	class implObj;

	//! "Internal" implementation.

	const ref<implObj> impl;

	//! Constructor
	listitemcontainerObj(const ref<implObj> &impl,
			     const ref<listitemlayoutmanagerObj::implObj> &l);

	//! Destructor
	~listitemcontainerObj();
};

LIBCXXW_NAMESPACE_END

#endif
