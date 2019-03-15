/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/peephole_style.H"
#include "x/w/text_param_literals.H"
#include "peephole/peephole_toplevel.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "peepholed_toplevel_listcontainer/handler.H"
#include "generic_window_handler.H"
#include "popup/popup_impl.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "shared_handler_data.H"
#include "x/w/impl/background_color.H"

LIBCXXW_NAMESPACE_START

create_p_t_l_popup_ret_t
do_create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 const function<create_p_t_l_t> &factory,
 const function<create_p_t_l_handler_t> &handler_factory)
{
	// First, the handler.

	auto parent_handler=ref<generic_windowObj::handlerObj>
		(&args.parent_element->get_window_handler());

	auto attachedto_info=popup_attachedto_info::create
		(rectangle{}, args.attached_to_style);

	auto popup_handler=handler_factory({
			args.topleft_color,
			args.bottomright_color,	{
				args.popup_type,
				args.popup_wm_class_instance,
				parent_handler,
				attachedto_info,
				"label"_theme_font,
				"label_foreground_color",
				args.parent_element
				->nesting_level+
				args.extra_nesting_level,
				args.popup_window_type,
				"",
			}});

	layout_implptr popup_listlayoutmanagerptr;

	// Create the popup's peephole, in case the popup is too big for the
	// screen.
	auto popup_toplevel_layoutmanager=create_peephole_toplevel
		(popup_handler,
		 args.popup_border,
		 args.topleft_color,
		 args.list_style.background_color,
		 args.popup_peephole_style,
		[&]
		 (const auto &peephole_container)
		 {
			 auto [c, toplevel_info]=factory(peephole_container,
							 attachedto_info);

			 popup_listlayoutmanagerptr=c->get_layout_impl();

			 return toplevel_info;
		 });

	// Now, finish creating the popup's implementation object, and the
	// popup "public" object itself.

	auto popup_impl=ref<popupObj::implObj>::create(popup_handler,
						       parent_handler);

	auto p=popup::create(popup_impl,
			     popup_toplevel_layoutmanager->impl,
			     popup_listlayoutmanagerptr);

	return { p, popup_impl->handler };
}

peepholed_toplevel_listcontainer_handler
create_p_t_l_handler(const peepholed_toplevel_listcontainer_handler_args &args)
{
	return peepholed_toplevel_listcontainer_handler::create(args);
}

LIBCXXW_NAMESPACE_END
