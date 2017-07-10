/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_container_impl_h
#define custom_combobox_container_impl_h

#include "combobox/custom_combobox_container.H"
#include "combobox/custom_combobox_popup_containerfwd.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popupfwd.H"
#include "nonrecursive_visibilityfwd.H"
#include "container_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN custom_combobox_containerObj::implObj
	: public nonrecursive_visibilityObj<container_elementObj
					    <child_elementObj>> {

	//! Alias

	typedef nonrecursive_visibilityObj<container_elementObj
					   <child_elementObj>> superclass_t;

 public:

	//! The peepholed container for the popup.

	//! We make sure to update its attachedto_info any time our position
	//! changes.
	const custom_combobox_popup_container popup_container;

	//! The handler for the attached popup.
	const ref<popup_attachedto_handlerObj> attachedto_handler;

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent_container,
		const custom_combobox_popup_container &popup_container,
		const ref<popup_attachedto_handlerObj> &attachedto_handler);

	//! Override initialize()

	//! The attachedto_info needs to be updated.
	void initialize(IN_THREAD_ONLY) override;

	//! Override process_updated_position().

	//! The attachedto_info needs to be updated.
	void process_updated_position(IN_THREAD_ONLY) override;

	//! Override absolute_location_updated().

	//! The attachedto_info needs to be updated.
	void absolute_location_updated(IN_THREAD_ONLY) override;

 private:

	//! Update the attachedto_info.

	//! So the popup is reposition to reflect our new absolute location.
	void update_attachedto_info(IN_THREAD_ONLY);
 public:
	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
