/*
** Copyright 2018-2020 Double Precision, Inc.
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
#include <x/w/input_field.H>
#include <x/w/impl/background_color_element.H>
#include <x/w/impl/container.H>
#include <x/w/impl/container_element.H>
#include <x/w/impl/child_element.H>
#include <x/w/impl/layoutmanager.H>

#include <string>
#include <iostream>

#include "close_flag.H"

// Custom container implementation object.
//
// Use the background_color_elementObj template mixin together with the
// container_elementObj mixin. The background_color_elementObj mixin keeps
// tab on the custom background color the container displays when it has
// pointer focus.
class my_custom_container_implObj
	: public x::w::background_color_elementObj<x::w::container_elementObj
					    <x::w::child_elementObj>> {

	// Alias for the superclass.

	typedef x::w::background_color_elementObj<x::w::container_elementObj
					   <x::w::child_elementObj>
					   > superclass_t;

	// Pretty color gradient for the background color.

	static x::w::radial_gradient custom_gradient()
	{
		x::w::radial_gradient g;

		g.inner_radius_axis=
			x::w::radial_gradient::radius_axis::shortest;
		g.inner_radius_axis=
			x::w::radial_gradient::radius_axis::shortest;

		g.gradient={ {0, {0, 0, 0}}, {1, {x::w::rgb::maximum/3,
						  x::w::rgb::maximum/3,
						  x::w::rgb::maximum/3}}};

		return g;
	}
public:

	// Initialize the background_color_elementObj mixin to the theme "30%"
	// color.

	my_custom_container_implObj(const x::w::container_impl &parent)
		: superclass_t{ custom_gradient(),

			// child_elementObj constructor parameter.
			parent}
	{
	}

	~my_custom_container_implObj()=default;

private:

	bool previous_pointer_focus=false;

	// Override pointer_focus()

	// pointer_focus() gets invoked when the pointer moves into or out of
	// the display element, or our custom container.

	void pointer_focus(ONLY IN_THREAD,
			   const x::w::callback_trigger_t &trigger) override
	{
		// Invoke the overridden pointer_focus().
		superclass_t::pointer_focus(IN_THREAD, trigger);


		// pointer_focus() also gets invoked when the pointer
		// moves in and out of any of the container's child elements.
		// switching background colors (may) trigger a redraw, which
		// is relatively expensive. Optimize things a bit, and update
		// our background color only when we actually lose or gain
		// pointer focus.

		auto new_pointer_focus=current_pointer_focus(IN_THREAD);

		if (new_pointer_focus==previous_pointer_focus)
			return;

		previous_pointer_focus=new_pointer_focus;

		// If the pointer was moved into, install our background color.
		// If the pointer was moved out of, remove our custom
		// background color.

		if (new_pointer_focus)
			set_background_color(IN_THREAD,
					     x::w::background_color_element<>
					     ::get(IN_THREAD));
		else
			remove_background_color(IN_THREAD);
	}
};

typedef x::ref<my_custom_container_implObj> my_custom_container_impl;

// Might as well have a custom public object.

class my_custom_containerObj : public x::w::containerObj {

public:

	const my_custom_container_impl impl; // My implementation object.

	// Constructor

	my_custom_containerObj(const my_custom_container_impl &impl,
			       const x::w::layout_impl &container_layout_impl)
		: x::w::containerObj{impl, container_layout_impl},
		impl{impl}
	{
	}

	~my_custom_containerObj()=default;
};

typedef x::ref<my_custom_containerObj> my_custom_container;

// Create a custom container object.
//
// The parent parameter comes from the factory. The caller provides the
// parent, and is responsible for invoking the factory's create_internally().

my_custom_container new_custom_container(const x::w::container_impl &parent)
{
	// Use it to construct our custom container
	// implementation object.

	auto impl=my_custom_container_impl::create(parent);

	// Create a new layout manager object.

	x::w::new_gridlayoutmanager new_container_lm;

	// Pass the new custom container to
	// new_container_lm.create() in order to construct
	// the internal layout manager implementation object
	// for the new container.

	x::w::layout_impl new_layout_impl=new_container_lm.create(impl);

	// Construct the container public object.

	return my_custom_container::create(impl, new_layout_impl);
}

void testcustomcontainer()
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

			 // Give ample, generous, 10mm padding for our
			 // custom container.

			 factory->padding(10.0);

			 // Obtain the parent container from the factory.

			 x::w::container_impl p=factory->get_container_impl();

			 auto new_container=new_custom_container(p);

			 // Before calling created_internally(), in order
			 // to complete the process, we should initialize
			 // the new container's contents.

			 x::w::gridlayoutmanager custom_grid_layout{
				 new_container->get_layoutmanager()
			 };

			 auto inner_factory=custom_grid_layout->append_row();

			 // Also give the sample, generous, 10mm padding around
			 // the input field in our custom container.

			 inner_factory->padding(10.0);

			 x::w::input_field_config config{10};

			 inner_factory->create_input_field("", config);

			 // And now that the custom container's contents
			 // are all set, formally install the custom container
			 // in *it's* container.

			 factory->created_internally(new_container);
		 },
		 x::w::new_gridlayoutmanager{});

	main_window->set_window_title("Custom container");

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
		testcustomcontainer();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
