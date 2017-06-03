/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholeobj_h
#define peepholeobj_h

#include "peephole/peepholefwd.H"
#include "x/w/containerobj.H"

LIBCXXW_NAMESPACE_START

//! The \ref peephole "peephole" display element.

class LIBCXX_HIDDEN peepholeObj : public containerObj {

 public:

	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	class layoutmanager_implObj;

	//! Constructor
	peepholeObj(const ref<implObj> &impl,
		    const ref<layoutmanager_implObj> &layout);

	//! Destructor
	~peepholeObj();
};

LIBCXXW_NAMESPACE_END

#endif
