/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_buttonobj_h
#define x_w_buttonobj_h

#include <x/w/buttonfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/hotspot_bgcolorobj.H>
#include <x/w/singletonlayoutmanagerfwd.H>

LIBCXXW_NAMESPACE_START

//! Button implementation object.

//! \see button

class buttonObj : public containerObj,
		  public focusableObj,
		  public hotspot_bgcolorObj {

public:
	class LIBCXX_HIDDEN implObj;

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

	focusable_impl get_impl() const override LIBCXX_INTERNAL;

	//! Override get_layout_impl().

	//! Returns the singleton layout manager with the button's content.
	layout_impl get_layout_impl() const override LIBCXX_HIDDEN;

	//! Soft override of get_layoutmanager().

	singletonlayoutmanager get_layoutmanager();

	//! Soft override of get_layoutmanager().

	const_singletonlayoutmanager get_layoutmanager() const;
};

LIBCXXW_NAMESPACE_END

#endif
