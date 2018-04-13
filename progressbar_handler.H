/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef progressbar_handler_h
#define progressbar_handler_h

#include "x/w/progressbar.H"
#include "x/w/dim_arg.H"
#include "x/w/rgbfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/child_elementobj.H"
#include "themedim_element_minoverridefwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! The display element for the slider.
//!
//! Overrides the minimum width, to ensure the slider is wide enough, as
//! requested.
//!
//! Overrides the default font and color for any labels in the progress bar.

class LIBCXX_HIDDEN progressbarObj::handlerObj
	: public themedim_element_minoverrideObj<
	container_elementObj<child_elementObj>> {

	//! Returned by label_theme_color()
	const color_arg foreground_color;
 public:

	//! Alias

	typedef themedim_element_minoverrideObj<
		container_elementObj<child_elementObj>> superclass_t;

	//! Constructor
	handlerObj(const container_impl &parent_container,
		   const progressbar_config &config);

	//! Destructor
	~handlerObj();

	//! Override label_theme_font()

	const char *label_theme_font() const override;

	//! Override label_theme_color()

	color_arg label_theme_color() const override;
};

LIBCXXW_NAMESPACE_END

#endif
