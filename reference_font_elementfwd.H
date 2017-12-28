/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef reference_font_elementfwd_H
#define reference_font_elementfwd_H

#include "reference_fontobj.H"

LIBCXXW_NAMESPACE_START

//! Template for attaching a reference_fontObj mixin to a display element.

//! The first template parameter is a subclass of elementObj::implObj,
//! and reference_font_elementObj multiply-inherits from the template parameter
//! and a \ref reference_fontObj via an intermediate
//! reference_font_tag template, using the tag specified by the optional
//! second template parameter.
//!
//! This makes it possible to have multiple reference elements, referencing
//! them as reference_font_tag<X>::reference_font,
//! reference_font_tag<Y>::reference_font.

template<typename element_impl, typename tag=void>
class reference_font_elementObj : public element_impl,
				  public reference_font_tag<tag> {

public:

	template<typename ...Args>
	reference_font_elementObj(const std::string_view
				  &fontname,
				  Args && ...args);

	template<typename ...Args>
	reference_font_elementObj(const current_fontcollection &reference_font,
				  Args && ...args);

	~reference_font_elementObj();

	elementObj::implObj &font_element() override;

	void initialize(IN_THREAD_ONLY) override;

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme) override;
};

LIBCXXW_NAMESPACE_END

#endif
