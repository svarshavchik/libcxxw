/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef tablelayoutmanager_impl_h
#define tablelayoutmanager_impl_h

#include "x/w/tablelayoutmanager.H"
#include "tablelayoutmanager/table_synchronized_axisfwd.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "x/w/gridlayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN tablelayoutmanagerObj::implObj
	: public listlayoutmanagerObj::implObj {

 public:
	//! The columns information.
	const table_synchronized_axis axis_impl;

	//! The grid layout manager for the header row.
	const ref<gridlayoutmanagerObj::implObj> header_layoutmanager_impl;

	//! Constructor
	implObj(const ref<listcontainer_pseudo_implObj> &container_impl,
		const ref<gridlayoutmanagerObj::implObj
		> &header_layoutmanager_impl,
		const list_element &list_element_singleton,
		const table_synchronized_axis &axis_impl);

	//! Override create_public_object
	layoutmanager create_public_object() override;

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
