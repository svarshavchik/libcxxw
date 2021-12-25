/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/appid.H>

#include <x/w/main_window.H>
#include <x/w/button_appearance.H>
#include <x/w/focus_border_appearance.H>
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
#include "customkeyfocus.H"

std::string x::appid() noexcept
{
	return "customkeyfocus.examples.w.libcxx.com";
}

// custom subclass of child_elementObj. Fixed metrics, 50x50 pixel size.

static inline auto create_child_element_init_params()
{
	x::w::child_element_init_params init_params;

	init_params.scratch_buffer_id=
		"my_element.customkeyfocus.examples.w.libcxx.com";

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

	my_element_implObj(const x::w::container_impl &parent_container)
		: my_element_implObj{parent_container,
			parent_container->container_element_impl()
			.get_screen()}
	{
	}

	my_element_implObj(const x::w::container_impl &parent_container,
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
		     const x::w::rectarea &areas) override;

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
// ourselves, but it's more optimal to call schedule_full_redraw(), which will
// draw us together with all the other display elements.

	schedule_full_redraw(IN_THREAD);
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
			schedule_full_redraw(IN_THREAD);
			return true;
		case 'g':
			current_color=green;
			schedule_full_redraw(IN_THREAD);
			return true;
		case 'b':
			current_color=blue;
			schedule_full_redraw(IN_THREAD);
			return true;
		}
	}

	return superclass_t::process_key_event(IN_THREAD, ke);
}

void my_element_implObj::do_draw(ONLY IN_THREAD,
				 const x::w::draw_info &di,
				 const x::w::rectarea &areas)
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

///////////////////////////////////////////////////////////////////////////
//
// Create just our custom element, in the window.

my_element create_just_my_element(const x::w::factory &f)
{
	auto impl=my_element_impl::create(f->get_container_impl());

	auto me=my_element::create(impl);

	f->created_internally(me);

	return me;
}

///////////////////////////////////////////////////////////////////////////
//
// Use the standard library input focus frame around our element. The
// library creates an input focus frame using its own building blocks. The
// input focus frame consists of internal library display elements in a
// container.

#include <x/w/impl/focus/standard_focusframecontainer_element_impl.H>
#include <x/w/impl/focus/standard_focusframecontainer_element.H>

my_element create_with_focusframe(const x::w::factory &f,
				  bool custom)
{
	// We get the factory where we're supposed to create our display
	// element. What we will do is create a container for the focus frame,
	// that's the element we'll create using the provided factory.

	auto f_c_impl=f->get_container_impl();

	x::w::border_infomm custom_focusoff_border;

	// Make our custom border extra thick.

	custom_focusoff_border.width=3.0;    // 3 millimeters wide
	custom_focusoff_border.height=3.0;   // 3 millimeters tall

	// For a custom input focus border to work correctly we need to
	// actually specify two borders: the border that's drawn when there's
	// no focus, and the border to draw when there's input focus.
	//
	// Now that the border width is set, copy the border structure.

	x::w::border_infomm custom_focuson_border=custom_focusoff_border;

	// Just set different colors for the custom borders.
	// The focus off border is invisible. This is done by specifying
	// an invisible color.

	custom_focusoff_border.color1=x::w::rgb{ 0, 0, 0, 0};

	// And the focus on color, yellow/green.

	custom_focuson_border.color1=x::w::rgb{ x::w::rgb::maximum/4*3,
						x::w::rgb::maximum/4*3,
						0};

	// Make our focus border.

	// Use standard theme borders. Hijack them from the buttons'
	// appearance objects.

	auto focus_border=x::w::normal_button().appearance->focus_border;

	if (custom)
	{
		focus_border=focus_border
			->modify([&]
				 (const auto &appearance)
				 {
					 appearance->focusoff_border=
						 custom_focusoff_border;
					 appearance->focuson_border=
						 custom_focuson_border;
				 });
	}

	auto focus_frame_container_impl=
		x::w::create_nonrecursive_visibility_focusframe_impl
		(f_c_impl,
		 focus_border,

		 // Additional padding between the focus frame and the
		 // display element.

		 0, 0
		 );

	// And our real custom display element will be a display element in
	// the focus frame container. So, we pass the focus frame container
	// implementation object as my_element's parent container:

	auto impl=my_element_impl::create(focus_frame_container_impl);

	auto me=my_element::create(impl);

	// We now create the "public object for the focus frame container.

	auto focusframe_container=
		x::w::create_focusframe_container(focus_frame_container_impl,
						  me);

	// We don't call created_internally() in this case, We created
	// my_element in that container, and create_focusframe_container()
	// takes care of acquiring the internal factory from the focus frame's
	// container, and calling created_internally() for the display element
	// in the focus frame container.

	// We need, however, to explicitly call show(). This is because we
	// created a non-recursive visibility implementation object for the
	// focus frame container. show_all() and hide_all() will not recurse
	// into the focus frame's display elements, just show or hide the
	// focus frame itself, as if it was a single, whole element. But it's
	// really not. So, in order to get the expected results we need to
	// make our custom display element visible we need to explicitly do so.
	//
	// A display element is visible only when all of its parent containers
	// are visible, in addition to itself. So, this display element's
	// visibility is tied to its parent container's, the focus frame
	// container.

	me->show();

	// However we, instead, need to call created_internally() for the
	// focus frame container's "public" object. After all, this is the
	// display element we ostensibly created for the factory that was
	// passed here.

	f->created_internally(focusframe_container);

	// Bonus: use label_for() to tell the focus frame container that it's
	// a label for our custom focusable element. This slightly improves
	// the user experience. As a result of this, clicking on the area
	// occupied by the focus frame moves keyboard input focus to the
	// custom element. The input focus frame does not occupy much size,
	// but this is consistent with the existing usage of the focus frame
	// user interface.
	focusframe_container->label_for(me);

	return me;
}

my_element create_my_element(const options &opts, const x::w::factory &f)
{
	if (opts.focusframe->value || opts.custom_focusframe->value)
		return create_with_focusframe(f, opts.custom_focusframe->value);

	return create_just_my_element(f);
}

void testcustomfocus(const options &opts)
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

			 auto layout=main_window->gridlayout();

			 layout->col_alignment(0, x::w::halign::center);
			 auto factory=layout->append_row();

			 auto c=create_my_element(opts, factory);

			 factory=layout->append_row();

			 auto b=factory->create_button("Enable/Disable");

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

	close_flag->wait();
}

int main(int argc, char **argv)
{
	try {
		options opts;

		opts.parse(argc, argv);
		testcustomfocus(opts);
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
