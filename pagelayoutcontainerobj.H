/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_pagelayoutcontainerobj_h
#define x_w_pagelayoutcontainerobj_h

#include "x/w/containerobj.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pagelayoutcontainer

class pagelayoutcontainerObj: public containerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	pagelayoutcontainerObj(const ref<implObj> &impl,
			       const layout_impl &container_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~pagelayoutcontainerObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
