/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusable_labelobj_h
#define x_w_focusable_labelobj_h

#include <x/w/focusable_labelfwd.H>
#include <x/w/textlabelobj.H>
#include <x/w/containerobj.H>
#include <x/w/focusableownerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see focusable_label

class focusable_labelObj : public containerObj,
			   public textlabelObj,
			   public focusableObj::ownerObj {

public:
	class LIBCXX_HIDDEN implObj;

	//! Constructor
	focusable_labelObj(const container_impl &container_impl,
			   const layout_impl &container_layout_impl,
			   const ref<textlabelObj::implObj> &label_impl,
			   const focusable_impl &focusable_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~focusable_labelObj() LIBCXX_HIDDEN;

	//! Inherited from focusableObj
	focusable_impl get_impl() const override LIBCXX_HIDDEN;

	//! Inherited from elementObj

	//! The minimum overrides are set for the label display element
	//! inside the focus frame.

	ref<elementObj::implObj> get_minimum_override_element_impl() override;
};

LIBCXXW_NAMESPACE_END

#endif
