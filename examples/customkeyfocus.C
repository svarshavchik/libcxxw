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
#include <x/w/element.H>
#include <x/w/focusableownerobj.H>
#include <x/w/button.H>
#include <x/w/screen.H>
#include <x/w/picture.H>

#include <x/w/impl/container.H>
#include <x/w/impl/focus/focusable_element.H>
#include <x/w/impl/child_element.H>

#include <string>
#include <iostream>

#include "close_flag.H"

// custom subclass of child_elementObj. Fixed metrics, 50x50 pixel size.

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

// Custom implementation subclass of child_elementObj. Use the
// focusable_elementObj template mixin to make the element eligible for
// keyboard input focus.

class my_element_implObj :
	public x::w::focusable_elementObj<x::w::child_elementObj> {

	// Convenient alias.

	typedef x::w::focusable_elementObj<x::w::child_elementObj> superclass_t;
public:

// Cache some colors that will be used to draw the display element.
//
// These solid color pictures get created by the constructor and used going
// forward. This is a little more optimum, as opposed to creating the
// picture on the fly, each time.

	const x::w::const_picture white;
	const x::w::const_picture light_grey;

	const x::w::const_picture red, green, blue;

private:

// Which color this custom element is using at the moment.

	x::w::const_picture current_color;

public:

// Have our screen object handy, before creating all the color pictures.
//
// Take the parent container implementation object, find its element, and
// get_screen().

	my_element_implObj(const x::ref<x::w::containerObj::implObj>
			   &parent_container)
		: my_element_implObj{parent_container,
			parent_container->container_element_impl()
			.get_screen()}
	{
	}

	my_element_implObj(const x::ref<x::w::containerObj::implObj> &parent_container,
			   const x::w::screen &s)
		: superclass_t{parent_container,
			create_child_element_init_params()},

		  // Create all the colors

		  white{s->create_solid_color_picture({
					  x::w::rgb::maximum,
					  x::w::rgb::maximum,
					  x::w::rgb::maximum})},
		  light_grey{s->create_solid_color_picture({
					  x::w::rgb::maximum/6*5,
					  x::w::rgb::maximum/6*5,
					  x::w::rgb::maximum/6*5})},
		  red{s->create_solid_color_picture({
					  x::w::rgb::maximum,
					  0,
					  0})},
		  green{s->create_solid_color_picture({
					  0,
					  x::w::rgb::maximum,
					  0})},
		  blue{s->create_solid_color_picture({
					  0,
					  0,
					  x::w::rgb::maximum })},

		  // Initial color used.
		  current_color{light_grey}
	{
	}

	~my_element_implObj()=default;

// Implement the basic do_draw() override from elementObj::implObj.

	void do_draw(ONLY IN_THREAD,
		     const x::w::draw_info &di,
		     const x::w::rectangle_set &areas) override;

// Focusable elements can override keyboard_focus() and process_key_event().

	void keyboard_focus(ONLY IN_THREAD,
			    const x::w::callback_trigger_t &trigger) override;
	bool process_key_event(ONLY IN_THREAD,
			       const x::w::key_event &ke) override;
};

void my_element_implObj::keyboard_focus(ONLY IN_THREAD,
					const x::w::callback_trigger_t &trigger)
{
	superclass_t::keyboard_focus(IN_THREAD, trigger);

// The custom element is 100% white when it gains input focus, and goes back
// to the initial grey color when it loses input focus.

	current_color=current_keyboard_focus(IN_THREAD) ?
		white:light_grey;

// Redraw this element. We can do the necessary work to call do_draw()
// ourselves, but it's more optimal to call schedule_redraw(), which will draw
// us together with all the other display elements.

	schedule_redraw(IN_THREAD);
}

bool my_element_implObj::process_key_event(ONLY IN_THREAD,
					   const x::w::key_event &ke)
{
// See if r, g, or b was pressed; if so change our color and redraw. Return
// true to indicate that this keypress has been processed.
//
// If process_key_event() does not handle the key event, it must forward the
// call to the overridden superclass, to execute the default handling for this
// key event.

	if (ke.keypress) {
		switch (ke.unicode) {
		case 'r':
			current_color=red;
			schedule_redraw(IN_THREAD);
			return true;
		case 'g':
			current_color=green;
			schedule_redraw(IN_THREAD);
			return true;
		case 'b':
			current_color=blue;
			schedule_redraw(IN_THREAD);
			return true;
		}
	}

	return superclass_t::process_key_event(IN_THREAD, ke);
}

void my_element_implObj::do_draw(ONLY IN_THREAD,
				 const x::w::draw_info &di,
				 const x::w::rectangle_set &areas)
{
	// This basic implementation of do_draw() redraws only the rectangles
	// that need redrawing, according to the display server.
	//
	// All drawing must be done with draw_using_scratch_buffer().
	//
	// But first, all the drawing operations must be clipped to this
	// element's visible areas.

	x::w::clip_region_set clipped{IN_THREAD, *this, di};

	// Draw each rectangle.
	for (const x::w::rectangle &r:areas)
	{
		draw_using_scratch_buffer
			(IN_THREAD,
			 [&, this]
			 (const x::w::picture &pic,
			  const x::w::pixmap &pix,
			  const x::w::gc &context)
			 {
				 // Now drawing the portion of the rectangle
				 // specified by rectangle r.
				 //
				 // This is irrelevant for solid color
				 // pictures, but if this were a gradient
				 // picture, the source coordinates must be
				 // given as r.x and r.y, because gradient
				 // colors get scaled across the display
				 // element, so this would compose the
				 // corresponding rectangle from the color
				 // gradient into the portion of the element
				 // being drawn now. The actual rectangle
				 // in the destination picture is always
				 // anchored in (0, 0), hence we specify it
				 // accordingly. This is the scratch buffer,
				 // that's given to us for redrawing this
				 // portion of the display element.

				 pic->composite(current_color,
						r.x,
						r.y,
						{0, 0, r.width, r.height});
			 },
			 r,
			 di, di,
			 clipped);
	}
}

typedef x::ref<my_element_implObj> my_element_impl;

// The public object for a focusable display element must multiply inherit
// from focusableObj::ownerObj. This is a custom subclass of elementObj.

class my_elementObj : public x::w::elementObj,
		      public x::w::focusableObj::ownerObj {

public:

	const my_element_impl impl; // My implementation object.

	// Constructor

	my_elementObj(const my_element_impl &impl)
		: x::w::elementObj{impl},
		  x::w::focusableObj::ownerObj{impl}, impl{impl}
	{
	}

	~my_elementObj()=default;
};

typedef x::ref<my_elementObj> my_element;

void testcustomfocus()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 main_window->set_background_color(
				       x::w::rgb{x::w::rgb::maximum/2,
						 x::w::rgb::maximum/2,
						 x::w::rgb::maximum/2});

			 x::w::gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 layout->col_alignment(0, x::w::halign::center);
			 auto factory=layout->append_row();

			 auto impl=my_element_impl::create(factory->get_container_impl());
			 auto c=my_element::create(impl);

			 factory->created_internally(c);

			 factory=layout->append_row();

			 auto b=factory->create_normal_button_with_label("Enable/Disable");

			 b->on_activate([c, status=true]
					(ONLY IN_THREAD,
					 const x::w::callback_trigger_t &trigger,
					 const x::w::busy &mcguffin)
					mutable {

						status=!status;

						c->set_enabled(status);
					});
		 },
		 x::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom focus");

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
		testcustomfocus();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
