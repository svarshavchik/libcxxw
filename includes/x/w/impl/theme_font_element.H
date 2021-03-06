/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_theme_font_element_H
#define x_w_impl_theme_font_element_H

#include <x/w/impl/theme_font_elementfwd.H>
#include <x/w/impl/theme_font.H>

LIBCXXW_NAMESPACE_START

//! Constructor

//! The first parameter specifies the reference font's name, the remaining
//! parameters get forwarded to the superclass.

template<typename element_impl, typename tag>
template<typename ...Args>
theme_font_1elementObj<element_impl, tag>
::theme_font_1elementObj(const font_arg &f,
			 Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  theme_font_tag<tag>(*this, f)
{
}

//! Constructor

//! \overload

template<typename element_impl, typename tag>
template<typename ...Args>
theme_font_1elementObj<element_impl, tag>
::theme_font_1elementObj(const current_fontcollection &reference_font,
			 Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  theme_font_tag<tag>(reference_font)
{
}

//! Default destructor
template<typename element_impl, typename tag>
theme_font_1elementObj<element_impl, tag>
::~theme_font_1elementObj()=default;

//! Inherited from theme_fontObj

//! Returns my attached widget.

template<typename element_impl, typename tag>
elementObj::implObj &theme_font_1elementObj<element_impl, tag>
::font_element()
{
	return this->get_element_impl();
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the theme_fontObj::initialized().

template<typename element_impl, typename tag>
void theme_font_1elementObj<element_impl, tag>
::initialize(ONLY IN_THREAD)
{
	// Initialize the mixin first.
	//
	// textlabelObj::implObj calls preferred_width(), which is implemented
	// in editor_implObj, which looks at the font's width, so the
	// theme_font needs to be initialized before we go down the
	// rabbit hole.
	theme_font_tag<tag>::initialize(IN_THREAD);
	element_impl::initialize(IN_THREAD);
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the theme_fontObj::theme_updated().

template<typename element_impl, typename tag>
void theme_font_1elementObj<element_impl, tag>
::theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme)
{
	theme_font_tag<tag>::theme_updated(IN_THREAD);
	element_impl::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
