/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_images_vectorobj_h
#define icon_images_vectorobj_h

#include "iconfwd.H"
#include "image.H"
#include "icon_imagefwd.H"
#include "connection_threadfwd.H"

#include <vector>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A mixin with a std::vector of icons.

class LIBCXX_HIDDEN icon_images_vectorObj : virtual public obj {

	//! All images this image_button_internal cycles through.
	std::vector<icon> icon_images_thread_only;

public:
	THREAD_DATA_ONLY(icon_images);

	//! Constructor
	icon_images_vectorObj(const std::vector<icon> &icon_images);

	//! Destructor
	~icon_images_vectorObj();

	//! initialize() all images.

	void initialize(IN_THREAD_ONLY);

	//! Reload all images after theme_updated().

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme);

	//! Resize the images
	void resize(IN_THREAD_ONLY, dim_t w, dim_t h, icon_scale scale);
};

LIBCXXW_NAMESPACE_END

#endif
