/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef date_input_field_handler_h
#define date_input_field_handler_h

#include "x/w/date_input_field.H"
#include "container_elementfwd.H"
#include "nonrecursive_visibilityfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN date_input_fieldObj::handlerObj
	: public nonrecursive_visibilityObj<container_elementObj
					    <child_elementObj>> {

	//! Parent superclass

	typedef nonrecursive_visibilityObj<container_elementObj
					   <child_elementObj>> superclass_t;

 public:
	//! Constructor
	handlerObj(const ref<containerObj::implObj> &parent_container);

	//! Destructor
	~handlerObj();
};

LIBCXXW_NAMESPACE_END

#endif
