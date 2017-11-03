/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_popup_container_impl_h
#define custom_combobox_popup_container_impl_h

#include "combobox/custom_combobox_popup_container.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "image_button_internalfwd.H"
#include "x/w/elementfwd.H"
#include <x/weakptr.H>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! Constructed by new_custom_comboboxlayoutmanager::create() in its
//! call to create_peepholed_toplevel_listcontainer_popup().
//!
//! It inherits from p_t_l_impl_t, and is returned by the first callback
//! that create_peepholed_toplevel_listcontainer_popup() passes to
//! create_popup().
//!
//! It is also passed to the constructor of
//! peepholed_toplevel_listcontainer_layoutmanager_implObj, the second value
//! returned by the same callback.
//!
//! Combines its automatically-computed width, and layout manager-provided
//! tallest combo-box popup item's height to derive the metrics for the
//! current combo-box selection element, if there's one installed.

class LIBCXX_HIDDEN custom_combobox_popup_containerObj::implObj
	: public p_t_l_impl_t {

	//! Alias

	typedef p_t_l_impl_t superclass_t;

	//! Set the current selection element's metrics.

	weakptr<elementptr> current_combobox_selection_element_thread_only;

	//! Set the combo-box button's element's size
	weakptr<image_button_internalptr> combobox_button_thread_only;

	THREAD_DATA_ONLY(current_combobox_selection_element);
	THREAD_DATA_ONLY(combobox_button);

 public:

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Set the default font for labels in the popup.
	const char *label_theme_font() const override;

	//! Override horizvert_updated

	//! Capture the horizontal metrics of the popup container.
	//! The horizontal metrics are copied to the combo-box's current
	//! selection, so that it is sized, horizontally, to be the same size
	//! as the dynamically-calculated popup list.

	void horizvert_updated(IN_THREAD_ONLY) override;

	//! Override update_tallest_row_height()

	//! Inherited from peepholed_toplevel_listcontainer_impl_elementObj.
	//! Invokes update_current_selection_metrics() if the tallest_row_height
	//! changed.

	bool update_tallest_row_height(IN_THREAD_ONLY,
				       dim_t new_tallest_height) override;

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
