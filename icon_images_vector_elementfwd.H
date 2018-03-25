/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_images_vector_elementfwd_h
#define icon_images_vector_elementfwd_h

#include "icon_images_vector.H"

LIBCXXW_NAMESPACE_START

//! Attach an icon_images_vector to an element.

//! The template parameter must be a subclass of elementObj::implObj.
//!
//! This template class multiply-inherits from the template parameter
//! and icon_images_vector.

template<typename element_impl>
class LIBCXX_HIDDEN icon_images_vector_elementObj
	: public element_impl,
	  public icon_images_vector {

 public:

	template<typename ...Args>
		icon_images_vector_elementObj(const std::vector<icon> &,
					      Args && ...args);

	icon_images_vector_elementObj();

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
