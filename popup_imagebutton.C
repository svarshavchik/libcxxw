/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup_imagebutton.H"
#include "focus/focusframecontainer_element.H"
#include "popup_imagebutton_impl.H"
#include "hotspot_element.H"
#include "image_button_internal.H"
#include "icon_images_vector_element.H"
#include "container_element.H"
#include "child_element.H"
#include "generic_window_handler.H"
#include "icon.H"

LIBCXXW_NAMESPACE_START

image_button_internal
do_create_popup_imagebutton(const gridfactory &f,
			    const function<popup_imagebutton_focusframe_factory>
			    &ff_factory,
			    const ref<elementObj::implObj> &popup_element,
			    const popup_imagebutton_config &config)
{
	// Visual appearance of the button.
	f->padding(0);
	f->border(config.grid_cell_border);
	f->halign(halign::fill);
	f->valign(valign::fill);

	auto parent_container=f->get_container_impl();

	// The actual element that will go into this factory will be the
	// focus frame, with the button inside it. First, construct
	// the focus frame implementation object.

	child_element_init_params focusframe_init_params{
		"focusframe@libcxx.com"
			};

	focusframe_init_params.background_color=
		config.grid_cell_background_color;

	auto cbfc=ff_factory(config.focusoff_border,
			     config.focuson_border,
			     parent_container,
			     focusframe_init_params);

	auto cbfc_container_impl=ref(&cbfc->get_container_impl());

	// The focus frame implementation object is the parent of the
	// popup button. Create its implementation button.

	auto &d=cbfc_container_impl->get_window_handler();

	auto icon1=d.create_icon_pixels(config.image1, render_repeat::none,
					0, 0,
					icon_scale::nomore);
	auto icon2=d.create_icon_pixels(config.image2, render_repeat::none,
					0, 0,
					icon_scale::nomore);

	auto image_button_internal_impl=
		ref<popup_imagebutton_implObj>
		::create(cbfc_container_impl,
			 std::vector<icon>{ icon1, icon2 },
			 popup_element);

	// We can now create the focusframe public object.

	auto ff=focusframecontainer::create(cbfc, image_button_internal_impl);

	// The focus frame's factory, where the focusable element, the
	// image button, gets created.
	auto focusframe_factory=ff->set_focusable();

	// Create the "public" object, show() it, and tell the focus frame:
	// here's what you hafe inside you.
	auto popup_button=image_button_internal
		::create(image_button_internal_impl);

	focusframe_factory->created_internally(popup_button);

	ff->show();

	ff->label_for(ff); // Make clicks on the focusframe work.

	f->created_internally(ff);

	return popup_button;
}


LIBCXXW_NAMESPACE_END
