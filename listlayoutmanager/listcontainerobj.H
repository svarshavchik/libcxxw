/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainerobj_h
#define listcontainerobj_h

#include "listlayoutmanager/listcontainerfwd.H"
#include "x/w/focusable_containerobj.H"

LIBCXXW_NAMESPACE_START

//! An object for a container with a listlayoutmanager

//! \see listlayoutmanager
class LIBCXX_HIDDEN listcontainerObj : public focusable_containerObj {
 public:

	class implObj;

	//! Internal implementation object.
	const focusable_impl list_focusable_impl;

	//! Constructor
	listcontainerObj(const focusable_impl &list_focusable_impl,
			 const container_impl &container_impl,
			 const ref<layoutmanagerObj::implObj> &list_impl);

	//! Destructor
	~listcontainerObj();

	//! Inherited from focusableObj
	focusable_impl get_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
