/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_internal_impl_h
#define image_button_internal_impl_h

#include "x/w/radio_groupfwd.H"
#include "image_button_internal.H"
#include "iconfwd.H"
#include "image.H"
#include "hotspot_elementfwd.H"
#include <vector>
#include "connection_threadfwd.H"

#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Imlementation object for the internal image button.

class LIBCXX_HIDDEN image_button_internalObj::implObj :
	public hotspot_elementObj<imageObj::implObj> {

	//! The superclass
	typedef hotspot_elementObj<imageObj::implObj> superclass_t;

	//! All images this image_button_internal cycles through.
	std::vector<icon> icon_images_thread_only;

	//! Container for the current index in icon_images

	typedef mpobj<size_t> current_image_t;

	//! Current index in icon_images
	current_image_t current_image;

public:
	THREAD_DATA_ONLY(icon_images);

	//! Constructor
	implObj(const ref<containerObj::implObj> &container,
		const std::vector<icon> &icon_images);

	//! Destructor
	~implObj();

	//! Inherited from elementObj::implObj

	void initialize(IN_THREAD_ONLY) override;

	//! Inherited from elementObj::implObj

	void theme_updated(IN_THREAD_ONLY) override;

	//! Switch to another icon.

	//! This is overridden by radio buttons, to automatically turn off
	//! all other radio buttons in its group.
	//!
	//! The default implementation calls do_set_image_number().

	virtual void set_image_number(IN_THREAD_ONLY, size_t);

	//! Actually update the current image number.

	//! Radio button code invokes this non-virtual function directly,
	//! for all radio buttons in its group. This avoids infinite
	//! recursion, since radio buttons override set_image_number().

	void do_set_image_number(IN_THREAD_ONLY, size_t);

	//! Return the current icon number.

	size_t get_image_number();

	//! Overridden from hotspotObj::implObj

	//! We do not use hotspot callbacks. Invoke set_image_number();

	void activated(IN_THREAD_ONLY) override;
};

//! Create an image button internal implementation display element.

//! The new display element has checkbox semantics.

ref<image_button_internalObj::implObj>
create_checkbox_impl(const ref<containerObj::implObj> &container,
		     const std::vector<icon> &icon_images) LIBCXX_HIDDEN;

//! Create an image button internal implementation display element.

//! The new display element has radio button semantics.

ref<image_button_internalObj::implObj>
create_radio_impl(const radio_group &group,
		  const ref<containerObj::implObj> &container,
		  const std::vector<icon> &icon_images) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
