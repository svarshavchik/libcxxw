/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_imagebutton_impl_h
#define popup_imagebutton_impl_h

#include "popup_imagebutton_implfwd.H"
#include "image_button_internal_impl.H"

LIBCXXW_NAMESPACE_START

/*! A popup show/hide button drawn using two images.

A subclass of image_button_internalObj::implObj, usually
a button that opens and closes the combo-box or the calendar date popup is an
\ref image_button_internal "internal image button object".

Subclasses image_button_internalObj::implObj
to show normal/highlighted image depending
upon the hotspot temperature.

Variable vertical metrics, the image icons get resized to the current height,
adjusting the horizontal metrics to keep track of them. The popup button gets
placed next to some element that's related to it, and it tracks to its height.

*/

class LIBCXX_HIDDEN popup_imagebutton_implObj
	: public image_button_internalObj::implObj
{

	typedef image_button_internalObj::implObj superclass_t;

 public:
	//! Constructor

	popup_imagebutton_implObj(const container_impl &container,
				  const std::vector<icon> &icon_images);

 private:

	//! Internal constructor

	popup_imagebutton_implObj(const container_impl &container,
				  const std::vector<icon> &icon_images,
				  dim_t first_icon_width,
				  dim_t first_icon_height);
 public:
	//! Destructor
	~popup_imagebutton_implObj();

	//! Override temperature_changed()

	//! Show image 1 when the pointer button is down, else show image 0.

	void temperature_changed(ONLY IN_THREAD,
				 const callback_trigger_t &trigger) override;

	//! Override activate_on_key()

	//! Also activate on cursor down (combo-box) and cursor right
	//! (date input fields).
	bool activate_on_key(const key_event &ke) override;

	//! Override initialize()

	//! Invoke current_position_updated to make sure our metrics are
	//! right.
	void initialize(ONLY IN_THREAD) override;

	//! Override current_position_updated

	//! We override our metrics to allow unbound height.
	//! Once resized, we resize the icons and fixate our horizontal
	//! metrics based on the width of the resized icons.

	void current_position_updated(ONLY IN_THREAD) override;

	//! Override update_image_metrics()

	//! We override the default metrics from imageObj::implObj based on
	//! the icon's width. The popup button is vertically resizable, so
	//! so the vertical metrics are adjustable, and the horizontal metrics
	//! are fixed to the icon's real width, as usual.
	void update_image_metrics(ONLY IN_THREAD) override;

 private:

	//! Called by initialize() and current_position_updated().

	void resize_button_icons(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
