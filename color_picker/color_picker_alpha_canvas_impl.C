/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker_alpha_canvas_impl.H"
#include "generic_window_handler.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/rgbfwd.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

static auto create_canvas_init_params(const container_impl &container)
{
	canvas_init_params params{{"color_picker_strip_width"},{},
				  "color_picker@libcxx.com"};

	params.background_color=image_color{"color-picker-alpha-background"};
	params.scratch_buffer_id="color_picker_alpha@libcxx.com";

	return params;
}

static color_arg create_color_gradient(const rgb &primary_color)
{
	auto begin_color=primary_color;
	auto end_color=primary_color;

	begin_color.a=0;
	end_color.a=rgb::maximum;

	return {linear_gradient{0, 0, 0, 1, 0, 0,
				{
				 {0, begin_color},
				 {1, end_color}}}};
}

color_picker_alpha_canvasObj::implObj::implObj(const container_impl &container,
					       const rgb &initial_color)
	: superclass_t{create_color_gradient(initial_color),
		       container,
		       create_canvas_init_params(container)}
{
}

color_picker_alpha_canvasObj::implObj::~implObj()=default;

void color_picker_alpha_canvasObj::implObj::update(ONLY IN_THREAD,
						   const rgb &color)
{
	background_color_element<>
		::update(IN_THREAD,
			 create_background_color(create_color_gradient(color)));
	schedule_full_redraw(IN_THREAD);
}

void color_picker_alpha_canvasObj::implObj
::cleared_to_background_color(ONLY IN_THREAD,
			      const picture &pic,
			      const pixmap &,
			      const gc &,
			      const draw_info &di,
			      const rectangle &r)
{
	pic->composite(background_color_element<>
		       ::get(IN_THREAD)->get_current_color(IN_THREAD),
		       r.x, r.y,
		       {0, 0, r.width, r.height},
		       render_pict_op::op_atop);
}

LIBCXXW_NAMESPACE_END
