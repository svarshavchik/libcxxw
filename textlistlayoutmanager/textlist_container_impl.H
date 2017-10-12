/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef textlist_container_impl_h
#define textlist_container_impl_h

#include "container_elementfwd.H"
#include "child_element.H"
#include "x/w/containerobj.H"
#include "themedimfwd.H"

LIBCXXW_NAMESPACE_START

//! The peepholed container with a \ref textlistlayoutmanager.

class LIBCXX_HIDDEN textlist_container_implObj :
	public container_elementObj<child_elementObj> {

 public:

	//! Alias
	typedef container_elementObj<child_elementObj> superclass_t;

	//! Constructor
	textlist_container_implObj(const ref<containerObj::implObj> &parent);

	//! Destructor
	~textlist_container_implObj();

	//! Implemented in the subclass.
	virtual const_themedim list_v_padding() const=0;

	//! Implemented in the subclass.
	virtual const_themedim list_left_padding() const=0;

	//! Implemented in the subclass.
	virtual const_themedim list_right_padding() const=0;
	//! Implemented in the subclass.

	virtual const_themedim list_inner_padding() const=0;
};

LIBCXXW_NAMESPACE_END

#endif
