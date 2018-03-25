/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_fontelement_h
#define peepholed_fontelement_h

#include "peephole/peepholed_fontelementfwd.H"
#include "peephole/peepholed_element.H"
#include "reference_font.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl, typename font_tag>
template<typename ...Args>
inline peepholed_fontelementObj<element_impl, font_tag>
::peepholed_fontelementObj(const font_impl_t &font_impl,
			   Args && ...args)
	: element_impl{std::forward<Args>(args)...},
	font_impl{font_impl}
{
}

template<typename element_impl, typename font_tag>
peepholed_fontelementObj<element_impl, font_tag>::~peepholed_fontelementObj()
=default;

template<typename element_impl, typename font_tag>
dim_t peepholed_fontelementObj<element_impl, font_tag>
::horizontal_increment(ONLY IN_THREAD) const
{
	return font_impl->font_nominal_width(IN_THREAD);
}

template<typename element_impl, typename font_tag>
dim_t peepholed_fontelementObj<element_impl, font_tag>
::vertical_increment(ONLY IN_THREAD) const
{
	return font_impl->font_height(IN_THREAD);
}

LIBCXXW_NAMESPACE_END

#endif
