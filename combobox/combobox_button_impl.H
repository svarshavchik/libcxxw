/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef combobox_button_impl_h
#define combobox_button_impl_h

#include "combobox/combobox_button_implfwd.H"
#include "image_button_internal_impl.H"

LIBCXXW_NAMESPACE_START

/*! Combo-box dropdown button

The drop-down button that opens and closes the combo-box popup is an
\ref image_button_internal "internal image button object".

Subclass the implementation object to show normal/highlighted image depending
upon the hotspot temperature.

*/

class LIBCXX_HIDDEN combobox_button_implObj
	: public image_button_internalObj::implObj {

 public:
	//! This is really a popup_attachedto_handlerObj.

	//! But all we need is to show/hide it.

	const ref<elementObj::implObj> popup_element_impl;

	//! Constructor

	combobox_button_implObj(const ref<containerObj::implObj> &container,
				const std::vector<icon> &icon_images,
				const ref<elementObj::implObj>
				&popup_element_impl);

	//! Destructor
	~combobox_button_implObj();

	//! Override temperature_changed()

	//! Show image 1 when the pointer button is down, else show image 0.

	void temperature_changed(IN_THREAD_ONLY) override;

	//! Override activated().

	//! show/hide the popup.

	void activated(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
