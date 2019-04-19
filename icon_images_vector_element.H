/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_images_vector_element_h
#define icon_images_vector_element_h

#include "icon_images_vector_elementfwd.H"
#include <utility>

LIBCXXW_NAMESPACE_START

//! Constructor

//! The first parameters gets forwarded to the icon_images_vector mixin.
//! The remaining parameters get forwarded to the template superclass.

template<typename element_impl>
template<typename ...Args>
icon_images_vector_elementObj<element_impl>
::icon_images_vector_elementObj(const std::vector<icon> &icons,
				Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  icon_images_vector(icons)
{
}

//! Default destructor.

template<typename element_impl>
icon_images_vector_elementObj<element_impl>::icon_images_vector_elementObj()
=default;

//! Override initialize().

//! Forwarded to the template superclass and the icon_images_vector mixin.

template<typename element_impl>
void icon_images_vector_elementObj<element_impl>::initialize(ONLY IN_THREAD)
{
	element_impl::initialize(IN_THREAD);
	icon_images_vector::initialize(IN_THREAD);
}

//! Override theme_updated().

//! Forwarded to the template superclass and the icon_images_vector mixin.

template<typename element_impl>
void icon_images_vector_elementObj<element_impl>
::theme_updated(ONLY IN_THREAD,
		const const_defaulttheme &new_theme)
{
	element_impl::theme_updated(IN_THREAD, new_theme);
	icon_images_vector::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
