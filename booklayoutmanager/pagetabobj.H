/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagetabobj_h
#define x_w_pagetabobj_h

#include "booklayoutmanager/pagetabfwd.H"
#include "x/w/containerobj.H"
#include "x/w/focusableownerobj.H"
#include "x/w/hotspotobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pagetab

class LIBCXX_HIDDEN pagetabObj : public containerObj,
		   public hotspotObj,
		   public focusableObj::ownerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pagetabObj(const ref<implObj> &impl,
		   const layout_impl &container_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~pagetabObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
