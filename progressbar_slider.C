/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "progressbar_slider.H"
#include "always_visible.H"
#include "container_element.H"
#include "background_color.H"
#include "background_color_element.H"
#include "element_screen.H"
#include "screen.H"
#include "defaulttheme.H"
#include "x/w/pixmap.H"
#include "x/w/picture.H"

LIBCXXW_NAMESPACE_START

progressbar_sliderObj
::progressbar_sliderObj(const ref<containerObj::implObj> &parent,
			const progressbar_config &config)
	: progressbar_sliderObj{parent, config,
		parent->container_element_impl().create_background_color
		(config.background_color)
		}
{
}

progressbar_sliderObj
::progressbar_sliderObj(const ref<containerObj::implObj> &parent,
			const progressbar_config &config,
			const background_color &color)

	// We temporary initialize the slider to 'color', and because
	// new_gradient_required we will make sure to create the gradient
	// in the connection thread.
	: superclass_t{color, color,
		parent, child_element_init_params{}, color},
	  slider_color{config.slider_color},
	  updated_theme{parent->container_element_impl()
			  .get_screen()->impl->current_theme.get()}
{
	// Throw an exception now, rather than later.

	if (std::holds_alternative<rgb_gradient>(slider_color))
		valid_gradient(std::get<rgb_gradient>(slider_color));
}

progressbar_sliderObj::~progressbar_sliderObj()=default;

void progressbar_sliderObj::initialize(IN_THREAD_ONLY)
{
	superclass_t::initialize(IN_THREAD);
	update(IN_THREAD);
}

void progressbar_sliderObj::process_updated_position(IN_THREAD_ONLY)
{
	superclass_t::process_updated_position(IN_THREAD);
	update(IN_THREAD);
}

void progressbar_sliderObj::update(IN_THREAD_ONLY)
{
	update(IN_THREAD, get_screen()->impl->current_theme.get());
}

void progressbar_sliderObj::theme_updated(IN_THREAD_ONLY,
					  const defaulttheme &theme)
{
	superclass_t::theme_updated(IN_THREAD, theme);

	update(IN_THREAD, theme);
}

void progressbar_sliderObj::update(IN_THREAD_ONLY,
				   const defaulttheme &current_theme)
{
	// See if any work is needed.

	if (value(IN_THREAD) == updated_value &&
	    maximum_value(IN_THREAD) == updated_maximum_value &&
	    data(IN_THREAD).current_position.width == updated_width &&
	    updated_theme == current_theme)
		return;

	bool new_gradient_required=
		updated_theme != current_theme
		|| data(IN_THREAD).current_position.width != updated_width;

	updated_value=value(IN_THREAD);
	updated_maximum_value=maximum_value(IN_THREAD);
	updated_width=data(IN_THREAD).current_position.width;
	updated_theme=current_theme;

	if (updated_width == 0)
		return;

	auto s=get_screen();

	if (new_gradient_required)
	{
#ifdef TEST_SLIDER_GRADIENT

		TEST_SLIDER_GRADIENT();
#endif
		background_color_element<progressbar_gradient_tag>
			::update(IN_THREAD, s->impl->create_background_color
				 (s->create_linear_gradient_picture
				  (updated_theme
				   ->get_theme_color_gradient(slider_color),
				   0, 0, coord_t::truncate(updated_width-1),
				   0)));

		new_gradient_required=false;
	}

	// Create a new picture buffer, 1 pixel height, same width as our
	// current width, pad-repeat it.
	auto h_pixmap=containerObj::implObj::get_window_handler()
		.create_pixmap(updated_width, 1);
	auto h_picture=h_pixmap->create_picture();

	h_picture->repeat(render_repeat::pad);

	h_picture->composite
		(background_color_element<progressbar_bgcolor_tag>
		 ::get(IN_THREAD)->get_current_color(IN_THREAD),
		 0, 0, 0, 0, updated_width, 1);

	// The slider position is updated_value/updated_maximum_value.
       auto v=updated_value;

       if (v > updated_maximum_value)
               v=updated_maximum_value;

	// Now compute the width of the slider bar, v/updated_maximum_value
	// of the updated_width.
	dim_t slider_end=dim_t::truncate
		( dim_t::value_type(updated_width) *
		  (updated_maximum_value > 0 ?
		   (double)v/updated_maximum_value
		   : 1.0));


	if (slider_end > 0)
		h_picture->composite(background_color_element
				     <progressbar_gradient_tag>
				     ::get(IN_THREAD)
				     ->get_current_color(IN_THREAD),
				     0, 0, 0, 0, slider_end, 1,
				     render_pict_op::op_over);

	set_background_color(IN_THREAD,
			     s->impl->create_background_color(h_picture));
}

LIBCXXW_NAMESPACE_END
