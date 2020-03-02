/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef itembutton_impl_h
#define itembutton_impl_h

#include "itemlayoutmanager/itembutton.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/always_visible_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class itembuttonObj::implObj
	: public always_visible_elementObj<container_elementObj
					   <child_elementObj>> {

	//! Alias
	typedef always_visible_elementObj<container_elementObj
					  <child_elementObj>> superclass_t;

public:
	//! Constructor
	implObj(const container_impl &parent_container,
		const child_element_init_params &init_params);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
