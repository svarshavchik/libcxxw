/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_fonts_current_fontcollectionobj_H
#define x_w_impl_fonts_current_fontcollectionobj_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/w/fontcollectionfwd.H>
#include <x/w/impl/fonts/current_fontcollectionfwd.H>
#include <x/w/impl/connection_threadfwd.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/screenfwd.H>
#include <x/w/font.H>
#include <x/w/types.H>

LIBCXXW_NAMESPACE_START

//! A current collection of fonts.

//! \see current_fontcollection

class LIBCXX_HIDDEN current_fontcollectionObj : virtual public obj {

 protected:

	//! The collection is for this font specification.
	font font_spec_thread_only;

	//! Current theme this collection is based on.

	//! When a theme gets updated we rebuild the font collection.
	//! The theme is cached here, so we know when the theme has changed.

	defaulttheme font_theme;

public:
	//! This font is for this screen.
	const screen font_screen;

	//! This font collection is for display elements with this alpha channel.
	const depth_t depth;

 private:

	//! The built font collection.

	fontcollection fc_thread_only;

	//! Create the new font collection

	fontcollection create_fc(const font &font_spec);

 public:

	//! Create a new font collection

	//! Internal entry point used by the font picker display element.

	static fontcollection create_fc(const font &font_spec,
					const screen &font_screen,
					const depth_t depth,
					const defaulttheme &font_theme);

	THREAD_DATA_ONLY(font_spec);
	THREAD_DATA_ONLY(fc);

	//! Constructor
	current_fontcollectionObj(const screen &font_screen,
				  depth_t depth,
				  const font &font_spec);
 protected:
	//! Internal constructor
	current_fontcollectionObj(const screen &font_screen,
				  depth_t depth,
				  const font &font_spec,
				  const defaulttheme &font_theme);
 public:
	//! Destructor
	~current_fontcollectionObj();

	//! Create a new \ref fontcollection "font collection".

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme);

 private:
	virtual void theme_was_really_updated(ONLY IN_THREAD);
};


LIBCXXW_NAMESPACE_END

#endif
