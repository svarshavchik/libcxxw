/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_container_impl_h
#define custom_combobox_container_impl_h

#include "combobox/custom_combobox_container.H"
#include "combobox/custom_combobox_popup_containerfwd.H"
#include "popup/popup_attachedto_handler_elementfwd.H"
#include "popup/popupfwd.H"
#include "nonrecursive_visibilityfwd.H"
#include "container_elementfwd.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN custom_combobox_containerObj::implObj
	: public popup_attachedto_handler_elementObj<
	nonrecursive_visibilityObj<container_elementObj
				   <child_elementObj>>> {

	//! Alias

	typedef popup_attachedto_handler_elementObj
		<nonrecursive_visibilityObj<container_elementObj
					    <child_elementObj>>> superclass_t;

 public:

	//! The peepholed container for the popup.

	//! We make sure to update its attachedto_info any time our position
	//! changes.
	const custom_combobox_popup_container popup_container;

	//! Constructor
	implObj(const ref<containerObj::implObj> &parent_container,
		const custom_combobox_popup_container &popup_container,
		const ref<popup_attachedto_handlerObj> &attachedto_handler);

	//! Destructor
	~implObj();

	//! Override label_theme_font()

	const char *label_theme_font() const override;
};

LIBCXXW_NAMESPACE_END

#endif
