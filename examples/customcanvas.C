/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/canvas.H>

#include <x/w/impl/canvas.H>
#include <x/w/impl/background_color_element.H>
#include <x/w/impl/scratch_and_mask_buffer_draw.H>
#include <x/w/impl/container.H>

#include <string>
#include <iostream>

#include "close_flag.H"

// Implementation object derives from the canvas element implementation object.
//
// Mixin templates:
//
// background_color_elementObj: attaches a background_color to the
// implementation object, the color to draw the circle with.
//
// scratch_and_mask_buffer_draw: simplified element drawing. Draw the entire
// element, and provide an additional masking pixmap.

struct fore_color_tag; // Tag for the background_color_element

class my_canvas_implObj : public x::w::scratch_and_mask_buffer_draw<
	x::w::background_color_elementObj<x::w::canvasObj::implObj,
					  fore_color_tag>> {

	// Alias for the superclass.

	typedef x::w::scratch_and_mask_buffer_draw<
		x::w::background_color_elementObj<x::w::canvasObj::implObj,
						  fore_color_tag>
		> superclass_t;
public:

	// Construction needs to take care of several details.

	// 1) Prepare a canvas_init_params for canvasObj::implObj's constructor,
	// that specifies the size of the display element, in millimeters
	//
	// 2) It would be nice if we could construct the masking scratch buffer
	// specifying the dimensions we expect to have. The preferred
	// element size is specified as 50x50 millimeters, and the scratch
	// buffer wants the size specified in pixels.
	//
	// Well, the canvas constructor computes the initial metrics for the
	// display element in pixels, so we can use this to initialize the
	// scratch buffer mixin.
	//
	// Start the ball rolling by constructing the canvas_init_params

	my_canvas_implObj(const x::ref<x::w::containerObj::implObj>
			  &parent_container)
		: my_canvas_implObj(parent_container,
				    x::w::canvas_init_params{
					    // Horizontal metrics:
					    {20.0, 50.0, 100.0},

					    // Vertical metrics
					    {20.0, 50.0, 100.0},

					    // Label ID for the custom
					    // element's scratch buffer.

					    "my_canvas@examples.w.libcxx.com"})
	{
	}

	// Now, we use x::w::canvasObj::implObj::create_child_element_params
	// to compute the metrics of the display elements in pixels.

	my_canvas_implObj(const x::ref<x::w::containerObj::implObj>
			  &parent_container,
			  const x::w::canvas_init_params &init_params)
		: my_canvas_implObj{parent_container,
			init_params,
			create_child_element_params(parent_container,
						    init_params)}
	{
	}

	// Now we have all the info we need.
	my_canvas_implObj(const x::ref<x::w::containerObj::implObj>
			  &parent_container,
			  const x::w::canvas_init_params &init_params,
			  const x::w::child_element_init_params
			  &child_init_params)
		: superclass_t{

		// scratch_and_mask_buffer_draw mixin constructor params:
		//
		// Label ID for the scratch mask.
		        "my_canvas_mask@examples.w.libcxx.com",

		// Initial dimensions of the scratch mask buffer.
		// We expect to open using the preferred pixel size.
			child_init_params.initial_metrics.horiz.preferred(),
			child_init_params.initial_metrics.vert.preferred(),


		// The background_color_element mixin constructor param:

		// The circle's color. Take color "0%" from the theme.

		        "0%",

		// Finally, the parameters to canvasObj::implObj's constructor:
			parent_container, init_params, child_init_params}
	{
	}

	~my_canvas_implObj()=default;

	// Implement do_draw().

	void do_draw(ONLY IN_THREAD,
		     const x::w::draw_info &di,
		     const x::w::picture &area_picture,
		     const x::w::pixmap &area_pixmap,
		     const x::w::gc &area_gc,
		     const x::w::picture &mask_picture,
		     const x::w::pixmap &mask_pixmap,
		     const x::w::gc &mask_gc,
		     const x::w::clip_region_set &clipped,
		     const x::w::rectangle &area_entire_rect) override;
};

typedef x::ref<my_canvas_implObj> my_canvas_impl;

// Draw our custom canvas display element.

void my_canvas_implObj::do_draw(ONLY IN_THREAD,
				const x::w::draw_info &di,

				// The picture, pixmap, and the graphic context
				// for the drawing area.

				const x::w::picture &area_picture,
				const x::w::pixmap &area_pixmap,
				const x::w::gc &area_gc,

				// The picture, pixmap, and the graphic context
				// for the 1-bit masking buffer.
				const x::w::picture &mask_picture,
				const x::w::pixmap &mask_pixmap,
				const x::w::gc &mask_gc,

				// A handle indicating that the drawing area
				// is ready, and clipped.
				const x::w::clip_region_set &clipped,

				// Give the current size of the display element.
				//
				// x & y are always 0, and width+height gives
				// the element's current size, in pixels.
				const x::w::rectangle &area_entire_rect)
{
	// We are responsible for clearing the masking buffer.

	x::w::gc::base::properties props;

	props.function(x::w::gc::base::function::CLEAR);
	mask_gc->fill_rectangle(area_entire_rect, props);

	// The circle's size is 1/10th the size of the display element.

	x::w::dim_t circle_width=area_entire_rect.width/10;
	x::w::dim_t circle_height=area_entire_rect.height/10;

	// Draw a filled in circle, then clear our the center of the circle.
	// We end up with the mask to draw the border with.
	//
	// Then we'll reduce the radius by twice the circle_width/height, and
	// clear the inner part of the circle.

	props.function(x::w::gc::base::function::SET);

	// Fill in a circle in the rectangle that, basically, encompasses our
	// entire drawing area.
	mask_gc->fill_arc(0, 0, area_entire_rect.width, area_entire_rect.height,
			  0, 360*64, props);

	if (circle_width > 0 && circle_height > 0)
	{
		// Subtract circle_width/height*2 from the element's size.
		//
		// This gives the width/height of the inner size of the circle.

		x::w::dim_t inner_width=area_entire_rect.width
			-circle_width-circle_width;

		x::w::dim_t inner_height=area_entire_rect.height
			-circle_height-circle_height;

		props.function(x::w::gc::base::function::CLEAR);

		// So we now clear a circle in a rectangle whose top-left
		// corner is (circle_width, circle_height), and whose size is
		// inner_width x inner_height.
		//
		// The library uses type-safe integer types like dim_t and
		// coord_t that are not only type-safe, but also guard against
		// overflows. Which is the reason why, above, the width/height
		// get subtracted twice, instead of subtracting width/height*2
		// (the multiplication results in a larger integer type.
		//
		// The first two parameters to fill_arc are coord_t-s, so
		// need to use truncate() in order to pass the dim_t-s.

		mask_gc->fill_arc(x::w::coord_t::truncate(circle_width),
				  x::w::coord_t::truncate(circle_height),
				  inner_width,
				  inner_height,
				  0, 360*64, props);
	}

	// Now retrieve the circle_color

	x::w::background_color current_color=
		x::w::background_color_element<fore_color_tag>::get(IN_THREAD);

	x::w::const_picture current_color_picture=
		current_color->get_current_color(IN_THREAD);

	// And use the mask to composite the circle into the scratch buffer,
	// which is already filled in with the default background color.

	area_picture->composite(current_color_picture,
				mask_picture,
				0, 0, // src_x, src_y
				0, 0, // mask_x, mask_y
				0, 0, // dst_x, dst_y
				area_entire_rect.width,
				area_entire_rect.height,
				x::w::render_pict_op::op_over);
}

// "Public" object subclasses the public canvas object. Not really needed
// here, just for completeness sake.

class my_canvasObj : public x::w::canvasObj {

public:

	const my_canvas_impl impl; // My implementation object.

	// Constructor

	my_canvasObj(const my_canvas_impl &impl)
		: x::w::canvasObj{impl}, impl{impl}
	{
	}

	~my_canvasObj()=default;
};

typedef x::ref<my_canvasObj> my_canvas;

void testcustomcanvas()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 x::w::gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 auto factory=layout->append_row();
			 factory->padding(0);

			 // Obtain the parent container from the factory.

			 x::ref<x::w::containerObj::implObj> parent_container=
				 factory->get_container_impl();

			 // Create the "implementation" object for the custom
			 // display element.

			 auto impl=my_canvas_impl::create(parent_container);

			 // Create the "public" object for the custom display
			 // element.
			 auto c=my_canvas::create(impl);

			 // Notify the factory that a new display element
			 // has been created, and it goes into its parent
			 // container.

			 factory->created_internally(c);

		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom canvas");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testcustomcanvas();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
