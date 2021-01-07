/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_images_vector_h
#define icon_images_vector_h

#include "x/w/impl/iconfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/rgbfwd.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! A mixin with a std::vector of icons.

class LIBCXX_HIDDEN icon_images_vector {

	//! All images this image_button_internal cycles through.
	std::vector<icon> icon_images_thread_only;

public:
	THREAD_DATA_ONLY(icon_images);

	//! Constructor
	icon_images_vector(const std::vector<icon> &icon_images);

	//! Destructor
	~icon_images_vector();

	//! initialize() all images.

	void initialize(ONLY IN_THREAD);

	//! Reload all images after theme_updated().

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);

	//! Resize the images
	void resize(ONLY IN_THREAD, dim_t w, dim_t h, icon_scale scale);
};

LIBCXXW_NAMESPACE_END

#endif
