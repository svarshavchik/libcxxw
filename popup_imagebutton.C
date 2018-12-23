/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup_imagebutton.H"
#include "x/w/impl/focus/focusframecontainer_element.H"
#include "x/w/impl/focus/standard_focusframecontainer_element.H"
#include "x/w/impl/always_visible_element.H"
#include "popup/popup_showhide_element.H"
#include "popup_imagebutton_impl.H"
#include "hotspot_element.H"
#include "image_button_internal.H"
#include "icon_images_vector_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/child_element.H"
#include "generic_window_handler.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_attachedto_handler_element.H"
#include "icon.H"

LIBCXXW_NAMESPACE_START

image_button_internal
create_standard_popup_imagebutton(const gridfactory &f,
				  const popup_attachedto_handler
				  &attachedto_handler,
				  const popup_imagebutton_config &config)
{
	return create_popup_imagebutton
		(f,
		 [&]
		 (const border_arg &focusoff_border,
		  const border_arg &focuson_border,
		  const container_impl &parent_container,
		  const child_element_init_params &init_params)
		 {
			 auto ff=ref<popup_attachedto_handler_elementObj
				     <popup_imagebutton_focusframe_implObj>>
				 ::create(attachedto_handler,
					  focusoff_border,
					  focuson_border,
					  0,
					  0,
					  parent_container,
					  parent_container,
					  init_params);

			 return ff;
		 },

		 attachedto_handler,
		 config);
}

image_button_internal
do_create_popup_imagebutton(const gridfactory &f,
			    const function<popup_imagebutton_focusframe_factory>
			    &ff_factory,
			    const popup_attachedto_handler &my_popup_handler,
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

	auto cbfc_container_impl=ref(&cbfc->focusframe_bordercontainer_impl()
				     .get_container_impl());

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
		ref<popup_showhide_elementObj<always_visible_elementObj<
			popup_imagebutton_implObj>>>
		::create(my_popup_handler, cbfc_container_impl,
			 std::vector<icon>{ icon1, icon2 });

	// Create the "public" object.
	auto popup_button=image_button_internal
		::create(image_button_internal_impl);

	// We can now create the focusframe public object.

	auto ff=create_focusframe_container_owner(cbfc, cbfc,
						  popup_button,
						  image_button_internal_impl);

	//! show() the public object.
	ff->show();

	ff->label_for(ff); // Make clicks on the focusframe work.

	f->created_internally(ff);

	return popup_button;
}


LIBCXXW_NAMESPACE_END
