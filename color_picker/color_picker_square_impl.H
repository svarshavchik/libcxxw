/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_square_impl_h
#define color_picker_square_impl_h

#include "color_picker/color_picker_square.H"
#include "x/w/impl/canvas.H"
#include "x/w/impl/background_color_elementfwd.H"
#include "x/w/rgb.H"

LIBCXXW_NAMESPACE_START

struct color_picker_h_gradient;
struct color_picker_v_gradient;

//! Internal implementation object.

class color_picker_squareObj::implObj
	: public background_color_elementObj<canvasObj::implObj,
					     color_picker_h_gradient,
					     color_picker_v_gradient> {


	//! Alias
	typedef background_color_elementObj<canvasObj::implObj,
					    color_picker_h_gradient,
					    color_picker_v_gradient
					    > superclass_t;
	//! Fixed color value

	//! This should have 0 for R, G, and B channels, except for the
	//! fixed value component.

	rgb fixed_color;

 public:
	//! Constructor

	//! The gradient gets initialized based on the fixed color and the
	//! color components specified for the horizontal and vertical
	//! gradients.
	//!
	//! \note
	//! \c fixed_color should have zeroes for the horizontal and vertical
	//! components.

	implObj(const container_impl &container,
		const rgb &fixed_color,
		rgb_component_t rgb::*horizontal_gradient_component,
		rgb_component_t rgb::*vertical_gradient_component,
		const canvas_init_params &canvas_params);

	//! Destructor
	~implObj();

	//! Draw the color square.

	void cleared_to_background_color(ONLY IN_THREAD,
					 const picture &pic,
					 const pixmap &pix,
					 const gc &context,
					 const draw_info &di,
					 const rectangle &r) override;

	//! Update my gradient.

	//! Given the currently selected color, and the color channels that
	//! should vary horizontally and vertically, recalculate the
	//! gradient.
	//!
	//! Note that only the new fixed color's components that does not
	//! correspond to the horizontal and vertical channels matters.
	void update(ONLY IN_THREAD,
		    const rgb &new_fixed_color,
		    rgb_component_t rgb::*horizontal_gradient_component,
		    rgb_component_t rgb::*vertical_gradient_component);

};

LIBCXXW_NAMESPACE_END

#endif
