/*
** Copyright 2017-2019 Double Precision, Inc.
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

class current_fontcollectionObj : virtual public obj {

 protected:

	//! The collection is for this font specification.
	font font_spec_thread_only;

public:
	//! This font is for this screen.
	const screen font_screen;

	//! This font collection is for widgets with this alpha channel.
	const depth_t depth;

 private:

	//! The built font collection.

	fontcollection fc_thread_only;

	//! Create the new font collection

	fontcollection create_fc(const font &font_spec,
				 const const_defaulttheme &font_theme);

 public:

	THREAD_DATA_ONLY(font_spec);
	THREAD_DATA_ONLY(fc);

	//! Mutex-protected copy of fc(IN_THREAD)

	//! fc(IN_THREAD) is used for speed, in the internal execution
	//! thread, which keeps fc_public updated.

	mpobj<fontcollection> fc_public;

	//! Constructor
	current_fontcollectionObj(const screen &font_screen,
				  depth_t depth,
				  const font &font_spec,
				  const const_defaulttheme &font_theme)
		LIBCXX_HIDDEN;

 public:
	//! Destructor
	~current_fontcollectionObj() LIBCXX_HIDDEN;

	//! Create a new \ref fontcollection "font collection".

	//! This is invoked from update_current_theme().
	virtual void current_theme_updated(ONLY IN_THREAD,
					   const const_defaulttheme &new_theme)
		LIBCXX_HIDDEN;
};


LIBCXXW_NAMESPACE_END

#endif
