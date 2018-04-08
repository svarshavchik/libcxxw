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
	const ref<focusableObj::implObj> focusable_impl;

	//! Constructor
	listcontainerObj(const ref<focusableObj::implObj> &focusable_impl,
			 const ref<containerObj::implObj> &container_impl,
			 const ref<layoutmanagerObj::implObj> &list_impl);

	//! Destructor
	~listcontainerObj();

	//! Inherited from focusableObj
	ref<focusableObj::implObj> get_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
