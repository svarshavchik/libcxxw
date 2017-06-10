/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef reference_font_elementfwd_H
#define reference_font_elementfwd_H

#include "reference_fontobj.H"

LIBCXXW_NAMESPACE_START

//! Attach a tag to a reference_fontObj

//! \see reference_font_elementObj
template<typename tag=void>
class reference_font_element : public reference_fontObj {
public:

	using reference_fontObj::reference_fontObj;

	~reference_font_element();
};

//! Template for attaching a reference_fontObj mixin to a display element.

//! The first template parameter is a subclass of elementObj::implObj,
//! and reference_font_elementObj multiply-inherits from the template parameter
//! and a \ref reference_fontObj via an intermediate
//! reference_font_element template, using the tag specified by the optional
//! second template parameter.
//!
//! This makes it possible to have multiple reference elements, referencing
//! them as reference_font_element<X>::reference_font,
//! reference_font_element<Y>::reference_font.

template<typename element_impl, typename tag=void>
class reference_font_elementObj : public element_impl,
				  public reference_font_element<tag> {

public:

	template<typename ...Args>
	reference_font_elementObj(const std::experimental::string_view
				  &fontname,
				  Args && ...args);

	template<typename ...Args>
	reference_font_elementObj(const current_fontcollection &reference_font,
				  Args && ...args);

	~reference_font_elementObj();

	void initialize(IN_THREAD_ONLY) override;

	void theme_updated(IN_THREAD_ONLY) override;
};

LIBCXXW_NAMESPACE_END

#endif
