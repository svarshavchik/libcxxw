/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "color_picker/color_picker_square_impl.H"
#include "x/w/impl/background_color_element.H"
#include "x/w/impl/container.H"
#include "x/w/scratch_buffer.H"

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
	  aux_scratch_buffer{container->container_element_impl()
			     .create_scratch_buffer
			     ("color_picker_square_aux@libcxx.com")},
	  mask_scratch_buffer{container->container_element_impl()
			      .create_alpha_scratch_buffer
			      ("color_picker_square_mask@libcxx.com")},
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
	rectangle scratch_rect{0, 0, r.width, r.height};

	aux_scratch_buffer->get
		(r.width, r.height,
		 [&]
		 (const auto &scratch_pic,
		  const auto &scratch_pix,
		  const auto &scratch_context)
		 {
			 // Fixed color, full opacity.

			 auto solid_color=fixed_color;

			 solid_color.a=rgb::maximum;

			 scratch_pic->fill_rectangle(scratch_rect,
						     solid_color);

			 // Now add the horizontal and the vertical
			 // components.

			 scratch_pic->composite
				 (background_color_element
				  <color_picker_h_gradient>
				  ::get(IN_THREAD)
				  ->get_current_color(IN_THREAD),
				  r.x, r.y, scratch_rect,
				  render_pict_op::op_add);

			 scratch_pic->composite
				 (background_color_element
				  <color_picker_v_gradient>
				  ::get(IN_THREAD)
				  ->get_current_color(IN_THREAD),
				  r.x, r.y, scratch_rect,
				  render_pict_op::op_add);

			 // Prepare the alpha channel mask.

			 mask_scratch_buffer->get
				 (1, 1,
				  [&]
				  (const auto &mask_pic,
				   const auto &mask_pix,
				   const auto &mask_context)
				  {
					  rgb mask;

					  mask.a=fixed_color.a;

					  mask_pic->repeat
						  (render_repeat::normal);

					  mask_pic->fill_rectangle
						  ({0, 0, 1, 1}, mask);

					  pic->composite
						  (scratch_pic,
						   mask_pic,
						   0, 0,
						   0, 0,
						   0, 0,
						   scratch_rect.width,
						   scratch_rect.height,
						   render_pict_op::op_atop);
				  });
		 });

}

LIBCXXW_NAMESPACE_END
