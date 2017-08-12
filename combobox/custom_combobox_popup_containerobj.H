/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_custom_combobox_popup_containerobj_h
#define x_w_custom_combobox_popup_containerobj_h

#include "combobox/custom_combobox_popup_containerfwd.H"
#include "listlayoutmanager/listcontainerobj.H"
#include "peephole/peepholed_toplevelobj.H"
#include "peephole/peepholed_toplevel_elementfwd.H"
#include "popup/popup_attachedto_infofwd.H"
#include "x/w/listlayoutmanagerobj.H"
#include "x/w/focusableownerobj.H"

LIBCXXW_NAMESPACE_START

//! Combo-box popup container object.

//! The combo-box popup is a top level window with a peephole. The combo-box
//! is placed in the peephole.
//!
//! \see custom_combobox_popup_container

class LIBCXX_HIDDEN custom_combobox_popup_containerObj
	: public peepholed_toplevel_elementObj<listcontainerObj> {

	//! Alias

	typedef peepholed_toplevel_elementObj<listcontainerObj> superclass_t;

	//! The implementation object is a focusable that needs an owner.

	//! We inherit from listcontainer which is not a focusable owner,
	//! so we need to keep the explicit object stashed around, as well.

	const ref<focusableObj::ownerObj> list_focusable_owner;
public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! The combo-box popup container uses the list layout manager.

	const ref<listlayoutmanagerObj::implObj> layout_impl;

	//! Constructor
	custom_combobox_popup_containerObj(const ref<implObj> &impl,
					   const ref<listlayoutmanagerObj
					   ::implObj> &layout_impl,
					   const popup_attachedto_info
					   &attachedto_info)
		LIBCXX_HIDDEN;

	//! Destructor
	~custom_combobox_popup_containerObj() LIBCXX_HIDDEN;

	//! Define peephole properties based on our attachedto_info.
	const popup_attachedto_info attachedto_info;

	//! Inherited from peepholed_toplevelObj

	//! Compute the values returned by max_width(), max_height(),
	//! horizontal_increment() and vertical_increment().
	void recalculate_peepholed_metrics(IN_THREAD_ONLY,
					   const screen &my_screen)
		override;

 private:
	//! Calculated by recalculate_metrics()
	dim_t max_width_value;
	//! Calculated by recalculate_metrics()
	dim_t max_height_value;
	//! Calculated by recalculate_metrics()
	dim_t horizontal_increment_value;
 public:
	//! Inherited from peepholed_toplevelObj
	dim_t max_width(IN_THREAD_ONLY) const override;

	//! Inherited from peepholed_toplevelObj
	dim_t max_height(IN_THREAD_ONLY) const override;

	//! Inherited from peepholed_toplevelObj
	dim_t horizontal_increment(IN_THREAD_ONLY) const override;

	//! Inherited from peepholed_toplevelObj
	dim_t vertical_increment(IN_THREAD_ONLY) const override;
};

LIBCXXW_NAMESPACE_END

#endif
