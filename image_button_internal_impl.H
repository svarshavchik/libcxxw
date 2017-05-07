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

LIBCXXW_NAMESPACE_START

//! Imlementation object for the internal image button.

class LIBCXX_HIDDEN image_button_internalObj::implObj :
	public hotspot_elementObj<imageObj::implObj> {

	//! The superclass
	typedef hotspot_elementObj<imageObj::implObj> superclass_t;

	//! All images this image_button_internal cycles through.
	std::vector<icon> icon_images_thread_only;

	//! Current index in icon_images
	size_t current_image_thread_only;

public:
	THREAD_DATA_ONLY(icon_images);
	THREAD_DATA_ONLY(current_image);

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

	void next_icon(IN_THREAD_ONLY, size_t);
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
