/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_imagebutton_impl_h
#define popup_imagebutton_impl_h

#include "popup_imagebutton_implfwd.H"
#include "popup/popup_showhide_elementfwd.H"
#include "image_button_internal_impl.H"
#include "always_visible_elementfwd.H"

LIBCXXW_NAMESPACE_START

/*! A popup show/hide button drawn using two images.

A button that opens and closes the combo-box or the calendar date popup is an
\ref image_button_internal "internal image button object".

Subclass the implementation object to show normal/highlighted image depending
upon the hotspot temperature.

*/

class LIBCXX_HIDDEN popup_imagebutton_implObj
	: public always_visible_elementObj<popup_showhide_elementObj
					   <image_button_internalObj::implObj>>
{

	typedef always_visible_elementObj<
		popup_showhide_elementObj<image_button_internalObj::implObj
					  >> superclass_t;

 public:
	//! Constructor

	popup_imagebutton_implObj(const ref<containerObj::implObj> &container,
				  const std::vector<icon> &icon_images,
				  const ref<elementObj::implObj>
				  &popup_element_impl);

 private:

	//! Internal constructor

	popup_imagebutton_implObj(const ref<containerObj::implObj> &container,
				  const std::vector<icon> &icon_images,
				  const ref<elementObj::implObj
				  > &popup_element_impl,
				  dim_t first_icon_width);
 public:
	//! Destructor
	~popup_imagebutton_implObj();

	//! Override temperature_changed()

	//! Show image 1 when the pointer button is down, else show image 0.

	void temperature_changed(IN_THREAD_ONLY,
				 const callback_trigger_t &trigger) override;

	//! Override activate_on_key()

	//! Also activate on cursor down (combo-box) and cursor right
	//! (date input fields).
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
	//! the icon's width. The popup button is vertically resizable, so
	//! so the vertical metrics are adjustable, and the horizontal metrics
	//! are fixed to the icon's real width, as usual.
	void update_image_metrics(IN_THREAD_ONLY) override;

 private:

	//! Called by initialize() and current_position_updated().

	void resize_button_icons(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
