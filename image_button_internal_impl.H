/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef image_button_internal_impl_h
#define image_button_internal_impl_h

#include "x/w/radio_groupfwd.H"
#include "x/w/image_buttonfwd.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/impl/always_visible_elementfwd.H"
#include "image_button_internal.H"
#include "icon_images_vector_elementfwd.H"
#include "iconfwd.H"
#include "image.H"
#include "hotspot_elementfwd.H"
#include <vector>

#include <x/mpobj.H>
#include <x/refptr_traits.H>

LIBCXXW_NAMESPACE_START

//! Parameters to image_button_internalObj::implObj's constructor

struct LIBCXX_HIDDEN image_button_internal_impl_init_params :
	public image_impl_init_params {

	std::vector<icon> icon_images;

	image_button_internal_impl_init_params
		(const container_impl &container,
		 const std::vector<icon> &icon_images);

	image_button_internal_impl_init_params
		(const container_impl &container,
		 const std::vector<icon> &icon_images,
		 const metrics::axis &horiz_metrics,
		 const metrics::axis &vert_metrics);

	~image_button_internal_impl_init_params();
};

//! Imlementation object for the internal image button.

class LIBCXX_HIDDEN image_button_internalObj::implObj :
	public always_visible_elementObj<hotspot_elementObj
					 <icon_images_vector_elementObj<
						  imageObj::implObj>>> {

	//! The superclass
	typedef always_visible_elementObj<hotspot_elementObj
					  <icon_images_vector_elementObj<
						  imageObj::implObj>
					   >> superclass_t;

	//! Container for the current index in icon_images

	typedef mpobj<size_t> current_image_t;

	//! Current index in icon_images
	current_image_t current_image;

	refptr_traits<image_button_callback_t>
		::ptr_t current_callback_thread_only;

public:
	THREAD_DATA_ONLY(current_callback);

	//! Constructor
	implObj(const image_button_internal_impl_init_params &init_params);

	//! Destructor
	~implObj();

	//! Switch to another icon.

	//! This is overridden by radio buttons, to automatically turn off
	//! all other radio buttons in its group.
	//!
	//! The default implementation calls do_set_image_number().

	virtual void set_image_number(ONLY IN_THREAD,
				      const callback_trigger_t &trigger,
				      size_t);

	//! Actually update the current image number.

	//! Radio button code invokes this non-virtual function directly,
	//! for all radio buttons in its group. This avoids infinite
	//! recursion, since radio buttons override set_image_number().

	void do_set_image_number(ONLY IN_THREAD, size_t);

	//! Return the current icon number.

	size_t get_image_number();
};

//! Create an image button internal implementation display element.

//! The new display element has checkbox semantics.

ref<image_button_internalObj::implObj>
create_checkbox_impl(const container_impl &container,
		     const std::vector<icon> &icon_images) LIBCXX_HIDDEN;

//! Create an image button internal implementation display element.

//! The new display element has radio button semantics.

ref<image_button_internalObj::implObj>
create_radio_impl(const radio_group &group,
		  const container_impl &container,
		  const std::vector<icon> &icon_images) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
