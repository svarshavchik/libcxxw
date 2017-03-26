/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef actionbutton_h
#define actionbutton_h

#include "child_elementobj.H"
#include "container_elementfwd.H"
#include "x/w/actionbuttonobj.H"

LIBCXXW_NAMESPACE_START

//! Action button implementation object.

class LIBCXX_HIDDEN actionbuttonObj::implObj
	: public container_elementObj<child_elementObj> {

 public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &container);

	//! Destructor
	~implObj();
};


LIBCXXW_NAMESPACE_END

#endif
