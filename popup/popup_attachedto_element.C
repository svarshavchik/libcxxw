/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "popup/popup_attachedto_elementfwd.H"
#include "popup/popup_handler.H"
#include "popup/popup_impl.H"
#include "peephole/peepholed_attachedto_container_impl.H"
#include "popup_imagebutton.H"
#include "shared_handler_data.H"
#include "peephole/peepholed_toplevel.H"
#include "gridlayoutmanager.H"
#include "x/w/element_popup_appearance.H"
#include "x/w/focus_border_appearance.H"

LIBCXXW_NAMESPACE_START

create_attachedto_element_ret_t
create_popup_attachedto_element_impl(factoryObj &parent_factory,
				     const const_element_popup_appearance
				     &appearance,
				     const function<create_popup_contents_impl_t
				     > &popup_contents_impl_factory,
				     const function<create_popup_contents_t
				     > &popup_contents_factory,
				     const function<void (const gridfactory &)
				     > &current_value_factory)
{
	auto parent_container=parent_factory.get_container_impl();

	auto attachedto_info=
		popup_attachedto_info::create(rectangle{},
					      attached_to::right_or_left);

	// Our container implementation object, for the current color field and
	// the popup button.
	auto real_impl=popup_attachedto_element_impl::create(parent_container);

	// We will use the grid layout manager, of course.

	ref<gridlayoutmanagerObj::implObj> glm_impl=
		new_gridlayoutmanager{}.create(real_impl);

	auto glm=glm_impl->create_gridlayoutmanager();

	// Before creating the button that shows the popup
	// we need to create the popup itself.

	auto parent_handler=ref(&parent_container->get_window_handler());

	auto attachedto_handler=
		ref<popupObj::handlerObj>::create
		(popup_handler_args{exclusive_popup_type,
				    "element_popup",
				    parent_handler,
				    attachedto_info,
				    appearance->toplevel_appearance,
				    parent_container->container_element_impl()
				    .nesting_level+2,
				    "popup_menu,dropdown_menu",
				    "",
		});

	auto popup_impl=ref<popupObj::implObj>::create(attachedto_handler,
						       parent_handler);

	peephole_style popup_peephole_style{peephole_algorithm::automatic,
					    peephole_algorithm::automatic,
					    halign::fill};

	// Create the contents of the popup.

	auto popup_lm=create_peephole_toplevel
		(attachedto_handler,
		 appearance->popup_border,
		 appearance->popup_background_color,
		 appearance->popup_scrollbars_background_color,
		 popup_peephole_style,
		 appearance->horizontal_scrollbar,
		 appearance->vertical_scrollbar,
		 [&]
		 (const container_impl &parent)
		 {
			 child_element_init_params c_params;

			 c_params.background_color=
				 appearance->popup_background_color;

			 auto impl=popup_contents_impl_factory(parent,
							       c_params);

			 new_gridlayoutmanager nglm;

			 auto glm_impl=nglm.create(impl);

			 auto c=popup_contents_factory(attachedto_info,
						       glm_impl);

			 c->show_all();
			 return c;
		 });

	auto popup=popup::create(popup_impl, popup_lm->impl,
				 popup_lm->impl);

	// Create the current value element.

	// We use the grid layout manager to draw the border around it
	// (config.border)...
	auto f=glm->append_row();
	f->padding(0);
	f->border(appearance->element_border);
	current_value_factory(f);

	// We can now create the popup button next to the current color
	// element.

	auto popup_imagebutton=create_standard_popup_imagebutton
		(f, popup,
		 {
			 appearance->element_border,
			 appearance->popup_background_color,
			 appearance->button_image1,
			 appearance->button_image2,
			 appearance->button_focus_border,
		 });

	return {real_impl, popup_imagebutton, glm, popup};
}

LIBCXXW_NAMESPACE_END
