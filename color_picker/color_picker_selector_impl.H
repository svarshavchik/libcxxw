/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_selector_impl_h
#define color_picker_selector_impl_h

#include "color_picker/color_picker_selector.H"
#include "peephole/peepholed_attachedto_container_impl.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN color_picker_selectorObj::implObj
	: public peepholed_attachedto_containerObj::implObj {

	//! Alias
	typedef peepholed_attachedto_containerObj::implObj superclass_t;

 public:

	//! Constructor
	implObj(const container_impl &parent,
		const child_element_init_params &init_params);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
