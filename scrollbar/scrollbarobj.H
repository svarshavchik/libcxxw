/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbarobj_h
#define scrollbarobj_h

#include "x/w/elementobj.H"
#include "x/w/focusableownerobj.H"
#include "scrollbar/scrollbarfwd.H"

LIBCXXW_NAMESPACE_START

class scrollbarObj : public containerObj,
		     public focusableObj::ownerObj {

public:

	class implObj;

	const ref<implObj> impl;

	scrollbarObj(const ref<implObj> &impl,
		     const ref<containerObj::implObj> &container_impl,
		     const ref<layoutmanagerObj::implObj> &layout_impl)
		LIBCXX_HIDDEN;

	~scrollbarObj() LIBCXX_HIDDEN;
};

LIBCXXW_NAMESPACE_END

#endif
