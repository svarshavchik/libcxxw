/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_square_impl.H"
#include "x/w/impl/background_color_element.H"

LIBCXXW_NAMESPACE_START

// Create gradient colors for the specified components.

static linear_gradient horizontal_gradient(rgb_component_t rgb::*component)
{
	rgb ending_value;

	ending_value.*component=rgb::maximum;

	return {0, 0, 1, 0, 0, 0, {{0, {}}, {1, ending_value}}};
}

static linear_gradient vertical_gradient(rgb_component_t rgb::*component)
{
	rgb ending_value;

	ending_value.*component=rgb::maximum;

	return {0, 0, 0, 1, 0, 0, {{0, {}}, {1, ending_value}}};
}

color_picker_squareObj::implObj
::implObj(const container_impl &container,
	  const rgb &fixed_color,
	  rgb_component_t rgb::*horizontal_gradient_component,
	  rgb_component_t rgb::*vertical_gradient_component,
	  const canvas_init_params &canvas_params)
	: superclass_t{horizontal_gradient(horizontal_gradient_component),
		vertical_gradient(vertical_gradient_component),
		container, canvas_params},
	fixed_color{fixed_color}
{
}

color_picker_squareObj::implObj::~implObj()=default;

void color_picker_squareObj::implObj
::update(ONLY IN_THREAD,
	 const rgb &new_fixed_color,
	 rgb_component_t rgb::*horizontal_gradient_component,
	 rgb_component_t rgb::*vertical_gradient_component)
{
	fixed_color=new_fixed_color;

	fixed_color.*horizontal_gradient_component=0;
	fixed_color.*vertical_gradient_component=0;

	background_color_element<color_picker_h_gradient>
		::update(IN_THREAD, create_background_color
			 (horizontal_gradient(horizontal_gradient_component)));

	background_color_element<color_picker_v_gradient>
		::update(IN_THREAD, create_background_color
			 (vertical_gradient(vertical_gradient_component)));
	schedule_full_redraw(IN_THREAD);
}

void color_picker_squareObj::implObj
::cleared_to_background_color(ONLY IN_THREAD,
			      const picture &pic,
			      const pixmap &pix,
			      const gc &context,
			      const draw_info &di,
			      const rectangle &r)
{
	// Clear to the fixed color. This is one
	// RGB component.
	pic->fill_rectangle(r, fixed_color);

	// Now add the horizontal and the vertical
	// components.

	pic->composite(background_color_element<color_picker_h_gradient>
		       ::get(IN_THREAD)->get_current_color(IN_THREAD),
		       r.x, r.y, r,
		       render_pict_op::op_add);

	pic->composite(background_color_element<color_picker_v_gradient>
		       ::get(IN_THREAD)->get_current_color(IN_THREAD),
		       r.x, r.y, r,
		       render_pict_op::op_add);
}

LIBCXXW_NAMESPACE_END
