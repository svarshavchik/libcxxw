/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_theme_fontobj_H
#define x_w_impl_theme_fontobj_H

#include <x/w/impl/theme_fontfwd.H>
#include <x/w/impl/fonts/current_fontcollectionfwd.H>
#include <x/w/impl/connection_threadfwd.H>
#include <x/w/impl/defaultthemefwd.H>
#include <x/w/elementobj.H>
#include <x/w/types.H>
#include <x/w/font_arg.H>
#include <string_view>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! A mixin for attaching a theme-based font to a display element.

//! Adds a \ref current_fontcollection "current_fontcollection" to a
//! display element. Provides a current_font class member, with a cached
//! nominal_width and height.
//!
//! \see theme_font_elementObj

class theme_fontObj : virtual public obj {

	//! The font
	current_fontcollection current_font_thread_only;

	//! Cached nominal_width()
	dim_t font_nominal_width_thread_only;

	//! Cached height()
	dim_t font_height_thread_only;

public:
	THREAD_DATA_ONLY(current_font);
	THREAD_DATA_ONLY(font_nominal_width);
	THREAD_DATA_ONLY(font_height);

	//! Constructor
	theme_fontObj(elementObj::implObj &e,
			  const font_arg &f);

	//! Constructor
	theme_fontObj(const current_fontcollection &current_font);

	//! Destructor
	~theme_fontObj();

	//! Return my element.
	virtual elementObj::implObj &font_element()=0;

	//! The mixin's element has been initialized

	//! Invokes theme_updated().
	void initialize(ONLY IN_THREAD);

	//! The theme was updated.

	//! Invokes the cached font's theme_updated().
	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme);
};

//! Attach a tag to a theme_fontObj

//! \see theme_font_tagObj
template<typename tag>
class theme_font_tag : public theme_fontObj {
public:

	using theme_fontObj::theme_fontObj;

	~theme_font_tag()=default;
};

LIBCXXW_NAMESPACE_END

#endif
