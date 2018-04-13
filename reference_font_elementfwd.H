/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef reference_font_elementfwd_H
#define reference_font_elementfwd_H

#include "reference_fontobj.H"

LIBCXXW_NAMESPACE_START

//! One tagged reference font

//! \see reference_font_elementObj

template<typename element_impl, typename tag>
class LIBCXX_HIDDEN reference_font_1elementObj
	: public element_impl,
	  public reference_font_tag<tag> {

public:

	template<typename ...Args>
	reference_font_1elementObj(const font_arg &fontname,
				  Args && ...args);

	template<typename ...Args>
	reference_font_1elementObj(const current_fontcollection &reference_font,
				  Args && ...args);

	~reference_font_1elementObj();

	elementObj::implObj &font_element() override;

	void initialize(ONLY IN_THREAD) override;

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme) override;
};

template<typename ...args> struct reference_font_element_types;

//! Helper template used by reference_font_elementObj

template<typename base, typename arg>
struct LIBCXX_HIDDEN reference_font_element_types<base, arg> {

	typedef reference_font_1elementObj<base, arg> type;
};

//! Helper template used by reference_font_elementObj

template<typename base, typename arg1, typename arg2, typename ...args>
struct LIBCXX_HIDDEN reference_font_element_types<base, arg1, arg2, args...>
{

	typedef reference_font_1elementObj
		<typename
		 reference_font_element_types<base, arg2, args...>
		 ::type, arg1> type;
};

//! Template for attaching reference_fontObj mixins to display elements.

//! The first template parameter is a subclass of elementObj::implObj.
//! reference_font_elementObj multiply-inherits from the template
//! parameter and a \ref reference_fontObj via an intermediate
//! reference_font_tag template, using the tag specified by the optional
//! second template parameter (which defaults to \c void ).
//!
//! The tag is used to create a distinct type only, and is not otherwise used.
//! Additional template parameters may follow, specifying additional tags.
//! This result in multiple inheritances, each one is referenced as
//! reference_font_tag<tag>.
//!
//! The end result is a multiple inheritance from the first template parameter
//! and reference_font_tag<T>, with T specified by the template
//! parameters to reference_font_elementObj. If none are specified, beyond
//! the parent class, a single inheritance from reference_font_tag<void>
//! results.
//!
//! Each reference_font_tag<T> inherits from
//! \ref reference_fontObj "reference_fontObj",
//! which implements methods for each mixed-in font.
//!
//! \par Constructors
//!
//! For each mixed-in font, there's an initial string or current_fontcollection
//! parameter that corresponds to the mixed-in font.
//! The remaining constructor parameters
//! get forwarded to the parent elementObj::implObj superclass.

template<typename base, typename arg1=void, typename ...args>
using reference_font_elementObj=
	typename reference_font_element_types<base, arg1, args...>::type;

LIBCXXW_NAMESPACE_END

#endif
