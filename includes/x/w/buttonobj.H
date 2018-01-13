/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_buttonobj_h
#define x_w_buttonobj_h

#include <x/w/buttonfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>
#include <x/w/hotspot_bgcolorobj.H>

LIBCXXW_NAMESPACE_START

//! Button implementation object.

//! \see button
//!
//! Internally derived from a container and a focusable object. The button
//! manages its own internal container. Do not use container methods, they're
//! for internal use only.

class buttonObj : public containerObj,
		  public focusableObj,
		  public hotspot_bgcolorObj {

public:
	class implObj;

	struct internal_construction_info;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	buttonObj(const ref<implObj> &,
		  const internal_construction_info &)
		LIBCXX_HIDDEN;

	//! Destructor
	~buttonObj() LIBCXX_HIDDEN;

	//! Return the focusable object inside the button.

	//! \internal

	ref<focusableImplObj> get_impl() const override LIBCXX_INTERNAL;
};

LIBCXXW_NAMESPACE_END

#endif
