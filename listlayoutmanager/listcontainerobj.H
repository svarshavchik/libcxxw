/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainerobj_h
#define listcontainerobj_h

#include "listlayoutmanager/listcontainerfwd.H"
#include "x/w/focusable_containerobj.H"
#include "x/w/listlayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

//! An object for a container with a listlayoutmanager

//! \see listlayoutmanager
class LIBCXX_HIDDEN listcontainerObj : public focusable_containerObj {
 public:

	class implObj;

	//! Internal implementation object.
	const ref<focusableImplObj> focusable_impl;

	//! Constructor
	listcontainerObj(const ref<focusableImplObj> &focusable_impl,
			 const ref<containerObj::implObj> &container_impl,
			 const ref<listlayoutmanagerObj::implObj> &list_impl);

	//! Destructor
	~listcontainerObj();

	//! Inherited from focusableObj
	ref<focusableImplObj> get_impl() const override;
};

LIBCXXW_NAMESPACE_END

#endif
