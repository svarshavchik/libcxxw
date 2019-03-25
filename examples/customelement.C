/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include <x/w/main_window.H>
#include <x/w/main_window_appearance.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/element.H>
#include <x/w/button.H>

#include <x/w/impl/child_element.H>
#include <x/w/impl/background_color_element.H>
#include <x/w/impl/scratch_and_mask_buffer_draw.H>
#include <x/w/impl/container.H>

#include <string>
#include <iostream>

#include "close_flag.H"

struct fore_color_tag; // Tag for the background_color_element

static inline auto create_child_element_init_params()
{
	x::w::child_element_init_params init_params;

	init_params.scratch_buffer_id="my_element@examples.w.libcxx.com";

	init_params.initial_metrics={
		{50, 50, 50},
		{50, 50, 50}
	};

	return init_params;
}

class my_element_implObj : public x::w::scratch_and_mask_buffer_draw<
	x::w::background_color_elementObj<x::w::child_elementObj,
					  fore_color_tag>> {

	// Alias for the superclass.

	typedef x::w::scratch_and_mask_buffer_draw<
		x::w::background_color_elementObj<x::w::child_elementObj,
						  fore_color_tag>
		> superclass_t;
public:

	my_element_implObj(const x::w::container_impl &parent_container)
		: superclass_t{

		// Label ID for the scratch mask.
		        "my_element_mask@examples.w.libcxx.com",

		// Background color will be a linear gradient

			x::w::linear_gradient{
				0, 0, 1, 1,
				0, 0,
				{{0, {0, 0, 0}},
				 {1, {x::w::rgb::maximum/4*3,
				      x::w::rgb::maximum/4*3,
				      x::w::rgb::maximum/4*3}
				 }
				}
			},

		// Finally, the parameters to child_elementObjs constructor:
			parent_container,
			create_child_element_init_params()}
	{
	}

	~my_element_implObj()=default;

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

typedef x::ref<my_element_implObj> my_element_impl;

void my_element_implObj::do_draw(ONLY IN_THREAD,
				const x::w::draw_info &di,
				const x::w::picture &area_picture,
				const x::w::pixmap &area_pixmap,
				const x::w::gc &area_gc,
				const x::w::picture &mask_picture,
				const x::w::pixmap &mask_pixmap,
				const x::w::gc &mask_gc,
				const x::w::clip_region_set &clipped,
				const x::w::rectangle &area_entire_rect)
{
	x::w::gc::base::properties props;

	props.function(x::w::gc::base::function::CLEAR);
	mask_gc->fill_rectangle(area_entire_rect, props);

	x::w::dim_t circle_width=area_entire_rect.width/10;
	x::w::dim_t circle_height=area_entire_rect.height/10;

	props.function(x::w::gc::base::function::SET);

	mask_gc->fill_arc(0, 0, area_entire_rect.width, area_entire_rect.height,
			  0, 360*64, props);

	if (circle_width > 0 && circle_height > 0)
	{
		x::w::dim_t inner_width=area_entire_rect.width
			-circle_width-circle_width;

		x::w::dim_t inner_height=area_entire_rect.height
			-circle_height-circle_height;

		props.function(x::w::gc::base::function::CLEAR);

		mask_gc->fill_arc(x::w::coord_t::truncate(circle_width),
				  x::w::coord_t::truncate(circle_height),
				  inner_width,
				  inner_height,
				  0, 360*64, props);
	}

	x::w::background_color current_color=
		x::w::background_color_element<fore_color_tag>::get(IN_THREAD);

	x::w::const_picture current_color_picture=
		current_color->get_current_color(IN_THREAD);

	area_picture->composite(current_color_picture,
				mask_picture,
				0, 0,
				0, 0,
				0, 0,
				area_entire_rect.width,
				area_entire_rect.height,
				x::w::render_pict_op::op_over);
}

// "Public" object subclasses the public element object. Not really needed
// here, just for completeness sake.

class my_elementObj : public x::w::elementObj {

public:

	const my_element_impl impl; // My implementation object.

	// Constructor

	my_elementObj(const my_element_impl &impl)
		: x::w::elementObj{impl}, impl{impl}
	{
	}

	~my_elementObj()=default;
};

typedef x::ref<my_elementObj> my_element;

static void add_toggle_button(const my_element &e,
			      const x::w::factory &f)
{
	auto b=f->create_button("+/-", x::w::default_button());

	b->on_activate([e, s=x::w::dim_t{50}]
		       (ONLY IN_THREAD,
			const x::w::callback_trigger_t &trigger,
			const x::w::busy &mcguffin)
		       mutable
		       {
			       s=x::w::dim_t{150}-s;

			       e->impl->get_horizvert(IN_THREAD)
				       ->set_element_metrics(IN_THREAD,
							     {s, s, s},
							     {s, s, s});
		       });
}

void testcustomelement()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	// Custom main window appearance, white background color

	x::w::main_window_config config;

	x::w::main_window_appearance appearance=config.appearance->clone();

	appearance->background_color=x::w::white;

	config.appearance=appearance;

	auto main_window=x::w::main_window::create
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 main_window->remove_background_color();
			 x::w::gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 layout->col_alignment(0, x::w::halign::center);

			 auto factory=layout->append_row();

			 // Obtain the parent container from the factory.

			 x::w::container_impl parent_container=
				 factory->get_container_impl();

			 // Create the "implementation" object for the custom
			 // display element.

			 auto impl=my_element_impl::create(parent_container);

			 // Create the "public" object for the custom display
			 // element.
			 auto c=my_element::create(impl);

			 // Notify the factory that a new display element
			 // has been created, and it goes into its parent
			 // container.

			 factory->created_internally(c);

			 // Create a button to resize the element
			 // on the next row

			 factory=layout->append_row();

			 add_toggle_button(c, factory);
		 },
		 x::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom element");

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

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		testcustomelement();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
