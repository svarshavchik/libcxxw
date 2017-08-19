/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef label_theme_font_elementfwd_H
#define label_theme_font_elementfwd_H

#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

//! A mixin for overriding label_theme_font().

//! The constructor's first parameter is the theme font name, the remaining
//! parameters gets forwarded to the superclass's constructor.

template<typename element_impl>
class label_theme_font_elementObj : public element_impl {

	//! Overridden font.
	const char *label_theme_font_;
public:

	//! Constructor

	template<typename ...Args>
	label_theme_font_elementObj(const char *label_theme_font_,
				    Args && ...args);

	//! Destructor
	~label_theme_font_elementObj();

	//! Override label_theme_font().

	const char *label_theme_font() const override;
};

LIBCXXW_NAMESPACE_END

#endif
