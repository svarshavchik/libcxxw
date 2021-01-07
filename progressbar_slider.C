/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_slider.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/background_color.H"
#include "x/w/impl/background_color_element.H"
#include "background_color_element_recalculated.H"
#include "screen.H"
#include "generic_window_handler.H"
#include "defaulttheme.H"
#include "x/w/pixmap.H"
#include "x/w/picture.H"
#include "x/w/progressbar_appearance.H"

LIBCXXW_NAMESPACE_START

progressbar_sliderObj
::progressbar_sliderObj(const container_impl &parent,
			const progressbar_config &config)
	// We temporary initialize the slider to 'color', and because
	// new_gradient_required we will make sure to create the gradient
	// in the connection thread.
	: superclass_t{config.appearance->background_color,
		       config.appearance->slider_color,
		       parent, child_element_init_params
		       {{}, {}, config.appearance->background_color}},
	  value_thread_only{config.value},
	  maximum_value_thread_only{config.maximum_value}
{
}

progressbar_sliderObj::~progressbar_sliderObj()=default;

void progressbar_sliderObj
::all_background_colors_were_recalculated(ONLY IN_THREAD)
{
	update(IN_THREAD);

#ifdef TEST_SLIDER_GRADIENT
	TEST_SLIDER_GRADIENT();
#endif
}

void progressbar_sliderObj::update(ONLY IN_THREAD)
{
	auto width=data(IN_THREAD).current_position.width;

	if (width == 0)
		return;

	auto s=get_screen();

	// Create a new picture buffer, 1 pixel height, same width as our
	// current width, pad-repeat it.
	auto h_pixmap=containerObj::implObj::get_window_handler()
		.create_pixmap(width, 1);
	auto h_picture=h_pixmap->create_picture();

	h_picture->repeat(render_repeat::pad);

	h_picture->composite
		(background_color_element<progressbar_bgcolor_tag>
		 ::get(IN_THREAD)->get_current_color(IN_THREAD),
		 0, 0, 0, 0, width, 1);

	// The slider position is updated_value/updated_maximum_value.
       auto v=value(IN_THREAD);

       if (v > maximum_value(IN_THREAD))
               v=maximum_value(IN_THREAD);

	// Now compute the width of the slider bar, v/updated_maximum_value
	// of the width.
	dim_t slider_end=dim_t::truncate
		( dim_t::value_type(width) *
		  (maximum_value(IN_THREAD) > 0 ?
		   (double)v/maximum_value(IN_THREAD)
		   : 1.0));


	if (slider_end > 0)
		h_picture->composite(background_color_element
				     <progressbar_gradient_tag>
				     ::get(IN_THREAD)
				     ->get_current_color(IN_THREAD),
				     0, 0, 0, 0, slider_end, 1,
				     render_pict_op::op_over);

	set_background_color(IN_THREAD,
			     create_new_background_color
			     (s,
			      container_element_impl().get_window_handler()
			      .drawable_pictformat,
			      h_picture));
}

LIBCXXW_NAMESPACE_END
