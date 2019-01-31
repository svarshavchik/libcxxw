/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholeobj_h
#define peepholeobj_h

#include "peephole/peepholefwd.H"
#include "x/w/peepholelayoutmanagerobj.H"
#include "x/w/containerobj.H"
#include "x/w/elementfwd.H"

LIBCXXW_NAMESPACE_START

//! The \ref peephole "peephole" display element.

class LIBCXX_HIDDEN peepholeObj : public containerObj {

 public:

	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	//! Constructor
	peepholeObj(const ref<implObj> &impl,
		    const ref<peepholelayoutmanagerObj::implObj> &layout);

	//! Destructor
	~peepholeObj();

	//! Peepholed element

	const element peepholed_element;
};

LIBCXXW_NAMESPACE_END

#endif
