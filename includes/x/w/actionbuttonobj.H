/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_actionbuttonobj_h
#define x_w_actionbuttonobj_h

#include <x/w/actionbuttonfwd.H>
#include <x/w/containerobj.H>
#include <x/w/focusableobj.H>
#include <x/w/factoryfwd.H>
#include <x/w/gridlayoutmanagerobj.H>
#include <x/w/hotspot_bgcolorobj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display action buttons

//! \see actionbutton

class actionbuttonObj : public containerObj,
			public focusableObj,
			public hotspot_bgcolorObj {

public:
	class implObj;

	struct internal_construction_info;

	//! Internal implementation details.

	const ref<implObj> impl;

	//! Constructor
	actionbuttonObj(const ref<implObj> &,
			const internal_construction_info &)
		LIBCXX_HIDDEN;

	//! Destructor
	~actionbuttonObj() LIBCXX_HIDDEN;

	//! Return the focusable object inside the action button.

	//! \internal

	ref<focusableImplObj> get_impl() const LIBCXX_INTERNAL;
};

LIBCXXW_NAMESPACE_END

#endif
