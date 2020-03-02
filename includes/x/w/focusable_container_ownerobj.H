/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_container_ownerobj_h
#define x_w_focusable_container_ownerobj_h

#include <x/w/focusable_container_ownerfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableownerobj.H>

LIBCXXW_NAMESPACE_START

//! A focusable container that owns its focusable implementation

//! \see focusable_container_owner

class focusable_container_ownerObj : public containerObj,
				     public focusableObj::ownerObj {

public:
	//! Constructor
	focusable_container_ownerObj(const ref<containerObj::implObj> &impl,
				     const layout_impl &my_layout_impl,
				     const focusable_impl &f_impl);

	//! Destructor
	~focusable_container_ownerObj();
};

LIBCXXW_NAMESPACE_END

#endif
