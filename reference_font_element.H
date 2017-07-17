/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef reference_font_element_H
#define reference_font_element_H

#include "reference_font_elementfwd.H"
#include "reference_font.H"

LIBCXXW_NAMESPACE_START

//! Destructor
template<typename tag>
reference_font_element<tag>::~reference_font_element()=default;

//! Constructor

//! The first parameter specifies the reference font's name, the remaining
//! parameters get forwarded to the superclass.

template<typename element_impl, typename tag>
template<typename ...Args>
reference_font_elementObj<element_impl, tag>
::reference_font_elementObj(const std::string_view &fontname,
			    Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  reference_font_element<tag>(*this, fontname)
{
}

//! Constructor

//! \overload

template<typename element_impl, typename tag>
template<typename ...Args>
reference_font_elementObj<element_impl, tag>
::reference_font_elementObj(const current_fontcollection &reference_font,
			    Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  reference_font_element<tag>(reference_font)
{
}

//! Default destructor
template<typename element_impl, typename tag>
reference_font_elementObj<element_impl, tag>
::~reference_font_elementObj()=default;

//! Inherited from reference_fontObj

//! Returns my attached element.

template<typename element_impl, typename tag>
elementObj::implObj &reference_font_elementObj<element_impl, tag>
::font_element()
{
	return *this;
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the reference_fontObj::initialized().

template<typename element_impl, typename tag>
void reference_font_elementObj<element_impl, tag>
::initialize(IN_THREAD_ONLY)
{
	// Initialize the mixin first.
	//
	// textlabelObj::implObj calls preferred_width(), which is implemented
	// in editor_implObj, which looks at the font's width, so the
	// reference_font needs to be initialized before we go down the
	// rabbit hole.
	reference_font_element<tag>::initialize(IN_THREAD);
	element_impl::initialize(IN_THREAD);
}

//! Inherited from elementObj::implObj

//! Invokes the superclass, and the reference_fontObj::theme_updated().

template<typename element_impl, typename tag>
void reference_font_elementObj<element_impl, tag>
::theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
{
	reference_font_element<tag>::theme_updated(IN_THREAD, new_theme);
	element_impl::theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END

#endif
