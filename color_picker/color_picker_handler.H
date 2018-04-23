/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_handler_h
#define color_picker_handler_h

#include "x/w/color_picker.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/nonrecursive_visibilityfwd.H"
#include "x/w/impl/child_elementobj.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN color_pickerObj::handlerObj
	: public nonrecursive_visibilityObj<container_elementObj
					     <child_elementObj>> {

	//! Alias

	typedef nonrecursive_visibilityObj<container_elementObj
					    <child_elementObj>> superclass_t;
 public:
	//! Constructor
	handlerObj(const container_impl &parent_container);

	//! Destructor
	~handlerObj();
};

LIBCXXW_NAMESPACE_END

#endif
