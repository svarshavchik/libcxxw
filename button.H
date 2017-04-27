/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef button_h
#define button_h

#include "child_elementobj.H"
#include "container_elementfwd.H"
#include "x/w/buttonobj.H"

LIBCXXW_NAMESPACE_START

//!  button implementation object.

class LIBCXX_HIDDEN buttonObj::implObj
	: public container_elementObj<child_elementObj> {

 public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &container);

	//! Destructor
	~implObj();
};


LIBCXXW_NAMESPACE_END

#endif
