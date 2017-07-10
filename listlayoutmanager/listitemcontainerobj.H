/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listitemcontainerobj_h
#define listitemcontainerobj_h

#include "x/w/containerobj.H"
#include "x/w/listlayoutmanagerfwd.H"
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

	//! Installed callback.
	const std::function<list_item_status_change_callback_t
			    > status_change_callback;
	//! Constructor
	listitemcontainerObj(const ref<implObj> &impl,
			     const ref<listitemlayoutmanagerObj::implObj> &l,
			     const std::function<
			     list_item_status_change_callback_t
			     > &status_change_callback);


	//! Destructor
	~listitemcontainerObj();
};

LIBCXXW_NAMESPACE_END

#endif
