/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef color_picker_alpha_canvas_impl_h
#define color_picker_alpha_canvas_impl_h

#include "color_picker/color_picker_alpha_canvas.H"
#include "x/w/impl/canvas.H"
#include "x/w/impl/background_color_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

//! The background color is set to the checkerboard pattern.
//! Uses a background_color_element mixin to also stash away a color
//! gradient, that varies in alpha, that gets composed over this background.

class color_picker_alpha_canvasObj::implObj
	: public background_color_elementObj<canvasObj::implObj> {

 public:
	typedef background_color_elementObj<canvasObj::implObj> superclass_t;

	//! Constructor
	implObj(const container_impl &container,
		const rgb &initial_color);

	//! Destructor
	~implObj();

	//! Draw the alpha gradient over the background color.
	void cleared_to_background_color(ONLY IN_THREAD,
					 const picture &,
					 const pixmap &,
					 const gc &,
					 const draw_info &,
					 const rectangle &) override;

	//! Update the variable gradient from this primary color.
	void update(ONLY IN_THREAD,
		    const rgb &color);
};

LIBCXXW_NAMESPACE_END

#endif
