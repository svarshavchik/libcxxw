/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_progressbar_h
#define x_w_progressbar_h

#include <x/w/progressbarfwd.H>
#include <x/w/progressbarobj.H>
#include <x/w/dim_arg.H>
#include <x/w/font_arg.H>
#include <x/w/border_arg.H>
#include <x/w/rgb.H>
#include <x/ref.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Progress bar configuration

//! \see progressbar

struct progressbar_config {

	//! The border around the progress bar.

	border_arg border="progressbar";

	//! The minimum width of the progress bar

	dim_arg minimum_width{"progressbar"};

	//! Background color

	color_arg background_color{"progressbar_background_color"};

	//! Foreground color (default color for text labels)

	color_arg foreground_color{"progressbar_foreground_color"};

	//! Default text label font.
	font_arg label_font{theme_font{"progressbar"}};

	//! The progress bar's slider's color.

	color_arg slider_color{"progressbar"};
};

LIBCXXW_NAMESPACE_END

#endif
