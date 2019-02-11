/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef custom_combobox_container_impl_h
#define custom_combobox_container_impl_h

#include "combobox/custom_combobox_container.H"
#include "combobox/custom_combobox_popup_containerfwd.H"
#include "x/w/impl/popup/popupfwd.H"
#include "x/w/impl/nonrecursive_visibilityfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_element.H"

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

	//! Constructor
	implObj(const container_impl &parent_container,
		const custom_combobox_popup_container &popup_container,
		const popup &attached_popup);

	//! Destructor
	~implObj();

	//! Override label_theme_font()

	const char *label_theme_font() const override;
};

LIBCXXW_NAMESPACE_END

#endif
