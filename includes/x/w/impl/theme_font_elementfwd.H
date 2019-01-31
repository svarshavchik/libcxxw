/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_theme_font_elementfwd_H
#define x_w_impl_theme_font_elementfwd_H

#include <x/w/impl/theme_fontobj.H>

LIBCXXW_NAMESPACE_START

template<typename element_impl, typename tag>
class LIBCXX_PUBLIC theme_font_1elementObj;

//! One tagged theme font

//! \see theme_font_elementObj

template<typename element_impl, typename tag>
class theme_font_1elementObj: public element_impl,
			      public theme_font_tag<tag> {

public:

	template<typename ...Args>
	theme_font_1elementObj(const font_arg &fontname,
			       Args && ...args);

	template<typename ...Args>
	theme_font_1elementObj(const current_fontcollection &reference_font,
			       Args && ...args);

	~theme_font_1elementObj();

	elementObj::implObj &font_element() override;

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;
};

template<typename ...args> struct LIBCXX_PUBLIC theme_font_element_types;

//! Helper template used by theme_font_elementObj

template<typename base, typename arg>
struct theme_font_element_types<base, arg> {

	typedef theme_font_1elementObj<base, arg> type;
};

//! Helper template used by theme_font_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct theme_font_element_types<base, arg1, arg2, args...>
{

	typedef theme_font_1elementObj
		<typename
		 theme_font_element_types<base, arg2, args...>
		 ::type, arg1> type;
};

//! Template for attaching theme_fontObj mixins to display elements.

//! The first template parameter is a subclass of elementObj::implObj.
//! theme_font_elementObj multiply-inherits from the template
//! parameter and a \ref theme_fontObj via an intermediate
//! theme_font_tag template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The tag is used to create a distinct type only, and is not otherwise used.
//! Additional template parameters may follow, specifying additional tags.
//! This result in multiple inheritances, each one is referenced as
//! theme_font_tag<tag>.
//!
//! The end result is a multiple inheritance from the first template parameter
//! and theme_font_tag<T>, with T specified by the template
//! parameters to theme_font_elementObj. If none are specified, beyond
//! the parent class, a single inheritance from theme_font_tag<void>
//! results.
//!
//! Each theme_font_tag<T> inherits from
//! \ref theme_fontObj "theme_fontObj",
//! which implements methods for each mixed-in font.
//!
//! \par Constructors
//!
//! For each mixed-in font, there's an initial string or current_fontcollection
//! parameter that corresponds to the mixed-in font.
//! The remaining constructor parameters
//! get forwarded to the parent elementObj::implObj superclass.

template<typename base, typename arg1=void, typename ...args>
using theme_font_elementObj=
	typename theme_font_element_types<base, arg1, args...>::type;

LIBCXXW_NAMESPACE_END

#endif
