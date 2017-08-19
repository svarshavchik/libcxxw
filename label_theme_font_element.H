/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_theme_font_element_H
#define label_theme_font_element_H

#include "x/w/namespace.H"
#include "label_theme_font_elementfwd.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
template<typename ...Args>
label_theme_font_elementObj<element_impl>
::label_theme_font_elementObj(const char *label_theme_font_,
			      Args && ...args)
	: element_impl(std::forward<Args>(args)...),
	  label_theme_font_(label_theme_font_)
{
}

template<typename element_impl>
label_theme_font_elementObj<element_impl>::~label_theme_font_elementObj()
=default;

template<typename element_impl>
const char *label_theme_font_elementObj<element_impl>::label_theme_font() const
{
	return label_theme_font_;
}

LIBCXXW_NAMESPACE_END

#endif
