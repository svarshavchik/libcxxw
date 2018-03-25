/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_images_set_element_h
#define icon_images_set_element_h

#include "icon.H"
#include "connection_threadfwd.H"
#include "icon_images_set_elementfwd.H"
#include <utility>

LIBCXXW_NAMESPACE_START

//! One of the base classes inherited by an icon_images_set_element.

//! Constructor
template<typename C>
icon_1tag<C>::
icon_1tag(const icon &i) : tagged_icon_thread_only(i)
{
}

//! Initialize the icon.
template<typename C>

inline void icon_1tag<C>::initialize(ONLY IN_THREAD)
{
	tagged_icon(IN_THREAD)=
		tagged_icon(IN_THREAD)->initialize(IN_THREAD);
}

//! Reload all images after theme_updated().
template<typename C>
inline void icon_1tag<C>::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	tagged_icon(IN_THREAD)=
		tagged_icon(IN_THREAD)->theme_updated(IN_THREAD,
						      new_theme);

}

//! Resize the icon.
template<typename C>
inline void icon_1tag<C>::resize(ONLY IN_THREAD, dim_t w, dim_t h,
				 icon_scale scale)
{
	tagged_icon(IN_THREAD)=
		tagged_icon(IN_THREAD)->resize(IN_THREAD, w, h, scale);
}

//! Constructor

template<typename element_impl, typename Tag1, typename ...Tags>
template<typename ...Args>
inline icon_images_set_element_implObj<element_impl, Tag1, Tags...>
::icon_images_set_element_implObj(const icon &i, Args && ...args)
	: superclass_t(std::forward<Args>(args)...),
	  icon_1tag<Tag1>(i)
{
}

//! Override initialize()

//! Also initialize the icon in icon_1tag.
template<typename element_impl, typename Tag1, typename ...Tags>
inline void icon_images_set_element_implObj<element_impl, Tag1, Tags...>
::initialize(ONLY IN_THREAD)
{
	superclass_t::initialize(IN_THREAD);
	icon_1tag<Tag1>::initialize(IN_THREAD);
}

//! Override theme_updated()

//! Also update the icon_1tag.

template<typename element_impl, typename Tag1, typename ...Tags>
inline void icon_images_set_element_implObj<element_impl, Tag1, Tags...>
::theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme)
{
	superclass_t::theme_updated(IN_THREAD, new_theme);
	icon_1tag<Tag1>::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
