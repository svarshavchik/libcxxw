/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef combobox_button_impl_h
#define combobox_button_impl_h

#include "combobox/combobox_button_implfwd.H"
#include "popup/popup_showhide_elementfwd.H"
#include "image_button_internal_impl.H"

LIBCXXW_NAMESPACE_START

/*! Combo-box dropdown button

The drop-down button that opens and closes the combo-box popup is an
\ref image_button_internal "internal image button object".

Subclass the implementation object to show normal/highlighted image depending
upon the hotspot temperature.

*/

class LIBCXX_HIDDEN combobox_button_implObj
	: public popup_showhide_elementObj<image_button_internalObj::implObj> {

	typedef popup_showhide_elementObj<image_button_internalObj::implObj
					  > superclass_t;

 public:
	//! Constructor

	combobox_button_implObj(const ref<containerObj::implObj> &container,
				const std::vector<icon> &icon_images,
				const ref<elementObj::implObj>
				&popup_element_impl);

 private:

	//! Internal constructor

	combobox_button_implObj(const ref<containerObj::implObj> &container,
				const std::vector<icon> &icon_images,
				const ref<elementObj::implObj
				> &popup_element_impl,
				dim_t first_icon_width);
 public:
	//! Destructor
	~combobox_button_implObj();

	//! Override temperature_changed()

	//! Show image 1 when the pointer button is down, else show image 0.

	void temperature_changed(IN_THREAD_ONLY,
				 const callback_trigger_t &trigger) override;

	//! Override activate_on_key()

	//! Also activate on cursot down.
	bool activate_on_key(const key_event &ke) override;

	//! Override initialize()

	//! Invoke current_position_updated to make sure our metrics are
	//! right.
	void initialize(IN_THREAD_ONLY) override;

	//! Override current_position_updated

	//! We override our metrics to allow unbound height.
	//! Once resized, we resize the icons and fixate our horizontal
	//! metrics based on the width of the resized icons.

	void current_position_updated(IN_THREAD_ONLY) override;

	//! Override update_image_metrics()

	//! We override the default metrics from imageObj::implObj based on
	//! the icon's width. Combobox's button is vertically resizable, so
	//! so the vertical metrics are adjustable, and the horizontal metrics
	//! are fixed to the icon's real width, as usual.
	void update_image_metrics(IN_THREAD_ONLY) override;

 private:

	//! Called by initialize() and current_position_updated().

	void resize_button_icons(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
