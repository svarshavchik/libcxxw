/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_popup_container_impl_h
#define custom_combobox_popup_container_impl_h

#include "combobox/custom_combobox_popup_container.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "always_visiblefwd.H"
#include "image_button_internalfwd.H"
#include "x/w/elementfwd.H"
#include <x/weakptr.H>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! The implementation object is given to
//! create_peephole_toplevel() and, as such, implements always_visible.
//!
//! Combines its automatically-computed width, and layout manager-provided
//! tallest combo-box popup item's height to derive the metrics for the
//! current combo-box selection element, if there's one installed.

class LIBCXX_HIDDEN custom_combobox_popup_containerObj::implObj
	: public always_visibleObj<listcontainerObj::implObj> {

	//! Alias

	typedef always_visibleObj<listcontainerObj::implObj> superclass_t;

	//! Current tallest row height

	dim_t tallest_row_height_thread_only=1;

	//! Set the current selection element's metrics.

	weakptr<elementptr> current_combobox_selection_element_thread_only;

	//! Set the combo-box button's element's size
	weakptr<image_button_internalptr> combobox_button_thread_only;

	THREAD_DATA_ONLY(current_combobox_selection_element);
	THREAD_DATA_ONLY(combobox_button);

 public:
	THREAD_DATA_ONLY(tallest_row_height);

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override horizvert_updated

	//! Capture the horizontal metrics of the popup container.
	//! The horizontal metrics are copied to the combo-box's current
	//! selection, so that it is sized, horizontally, to be the same size
	//! as the dynamically-calculated popup list.

	void horizvert_updated(IN_THREAD_ONLY) override;

	//! The layout manager captures the tallest row's height, and calls me.

	void update_tallest_row_height(IN_THREAD_ONLY,
				       dim_t new_tallest_height);

	//! Install a link to the current combobox selection element.

	//! After the popup get constructed, and the combobox container
	//! with the current selection element gets constructed, link the
	//! popup to the current combobox selection element to update its
	//! minimum size.
	//!
	//! This combobox container contains a reference to the popup window,
	//! which contains a reference to this container. The combobox
	//! selection element is in the combobox container, so the combobox
	//! selection element has a reference to the combobox contanier, so
	//! we must use a weak reference to avoid a circular reference.

	void set_current_combobox_selection_element_and_button
		(const element &e,
		 const image_button_internal &button);
 private:

	//! Update current selection metrics.
	void update_current_selection_metrics(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
