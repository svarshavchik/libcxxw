/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_elementfwd_h
#define peepholed_toplevel_listcontainer_elementfwd_h

#include <x/ptrfwd.H>
#include "popup/popup_attachedto_infofwd.H"
#include "peepholed_toplevel_listcontainer/impl.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/screenfwd.H"
#include "x/w/focusableobj.H"

LIBCXXW_NAMESPACE_START

//! Mixin for a container with a listlayout manager in a top-level peephole.

//! A mixin used by a container with a listlayoutmanager in the top level
//! peephole of a popup.
//!
//! The template parameter inherits from the
//! \ref peepholed_toplevel_elementObj "peepholed_toplevel_elementObj"
//! mixin that inherits from a \ref listcontainerObj "listcontainerObj".
//!
//! Implements recalculate_peepholed_metrics(), max_width(), and max_height()
//! using a popup_attachedto_info, after inheriting from the template parameter.
//!
//! Also implements horizontal_increment() and vertical_increment() that's
//! inherited from \ref peepholedObj "peepholedObj".
//!
//! The constructor's first parameter is a popup_attachedto_info, which is
//! saved in the class member.
//!
//! The constructor's second parameter is a reference to the companion
//! implementation object, a reference to a
//! \ref peepholed_toplevel_listcontainer_implObj "peepholed_toplevel_listcontainer_implObj".
//!
//! The third parameter is the focusable object representing the focusable
//! list in the container.
//!
//! The remaining constructor parameters get
//! forwarded to the superclass's constructor.

template<typename element_impl>
class peepholed_toplevel_listcontainer_elementObj
	: public element_impl {

	//! Calculated by recalculate_peepholed_metrics()
	dim_t max_width_value;

	//! Calculated by recalculate_peepholed_metrics()
	dim_t max_height_value;

	//! The implementation object is a focusable that needs an owner.

	//! We inherit from listcontainer which is not a focusable owner,
	//! so we need to keep the explicit object stashed around, as well.

	const ref<focusableObj::ownerObj> list_focusable_owner;

public:

	//! Our popup_attachedto_info object.

	const popup_attachedto_info attachedto_info;

	//! The implementation mixin.

	const ref<peepholed_toplevel_listcontainer_implObj
		  > toplevel_listcontainer_impl;

	template<typename ...Args>
	peepholed_toplevel_listcontainer_elementObj
	(const popup_attachedto_info &attachedto_info,
	 const ref<peepholed_toplevel_listcontainer_implObj
	 > &toplevel_listcontainer_impl,
	 const focusable_impl &focusable_impl,
	 Args && ...args);

	~peepholed_toplevel_listcontainer_elementObj();

	void recalculate_peepholed_metrics(ONLY IN_THREAD, const screen &s)
		override;

	dim_t max_width(ONLY IN_THREAD) const override;

	dim_t max_height(ONLY IN_THREAD) const override;

private:
	//! Calculated by recalculate_metrics()
	dim_t horizontal_increment_value;

public:
	dim_t horizontal_increment(ONLY IN_THREAD) const override;

	dim_t vertical_increment(ONLY IN_THREAD) const override;
};

LIBCXXW_NAMESPACE_END

#endif
