/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholeobj_h
#define peepholeobj_h

#include "peephole/peepholefwd.H"
#include "x/w/containerobj.H"
#include "x/w/elementfwd.H"

LIBCXXW_NAMESPACE_START

//! The \ref peephole "peephole" display element.

class LIBCXX_HIDDEN peepholeObj : public containerObj {

 public:

	class implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

	class LIBCXX_HIDDEN layoutmanager_implObj;

	//! Constructor
	peepholeObj(const ref<implObj> &impl,
		    const ref<layoutmanager_implObj> &layout);

	//! Destructor
	~peepholeObj();

	//! Return the element in the peephole.

	element get_peepholed() const;
};

LIBCXXW_NAMESPACE_END

#endif
