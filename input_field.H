/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef input_field_h
#define input_field_h

#include "x/w/input_fieldobj.H"
#include "container_elementfwd.H"
#include "child_elementobj.H"
#include "editorfwd.H"
#include "nonrecursive_visibilityfwd.H"

LIBCXXW_NAMESPACE_START

//! \ref input_field "input_field" implementation class.

class LIBCXX_HIDDEN input_fieldObj::implObj
	: public nonrecursive_visibilityObj<container_elementObj<child_elementObj>> {

 public:

	typedef nonrecursive_visibilityObj<container_elementObj
					   <child_elementObj>> superclass_t;

	using superclass_t::superclass_t;

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
