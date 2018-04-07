/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_style.H"
#include "peephole/peephole_toplevel.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "peepholed_toplevel_listcontainer/handler.H"
#include "generic_window_handler.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_impl.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "shared_handler_data.H"
#include "x/w/impl/background_color.H"

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

//! Override get_layout_impl() in create_peepholed_toplevel_listcontainer_popup)_'s popup.

//! The popup returned by create_peepholed_toplevel_listcontainer_popup()
//! gets its get_layout_impl() overwritten to return the list layout manager
//! of the popup's list container, rather than its actual top level manager,
//! the one for the peephole.

class LIBCXX_HIDDEN peepholed_toplevel_listcontainer_popupObj
	: public popupObj {

 public:

	const ref<layoutmanagerObj::implObj> layout_impl;

	peepholed_toplevel_listcontainer_popupObj
		(const ref<implObj> &impl,
		 const ref<layoutmanagerObj::implObj> &layout,
		 const ref<layoutmanagerObj::implObj> &listlayout_impl)
		: popupObj{impl, layout},
		layout_impl{listlayout_impl}
		{
		}

	~peepholed_toplevel_listcontainer_popupObj()=default;

	ref<layoutmanagerObj::implObj> get_layout_impl() const override
	{
		return layout_impl;
	}
};

#if 0
{
#endif
}

popup_type_t create_exclusive_popup()
{
	return {&shared_handler_dataObj::opening_exclusive_popup,
			&shared_handler_dataObj::closing_exclusive_popup};
}

popup_type_t create_menu_popup()
{
	return {&shared_handler_dataObj::opening_menu_popup,
			&shared_handler_dataObj::closing_menu_popup};
}

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

	auto [opened_popup, closed_popup]=(*args.popup_type)();

	auto popup_handler=handler_factory({
			args.topleft_color,
			args.bottomright_color,	{
				opened_popup,
				closed_popup,
				args.popup_wm_class_instance,
				parent_handler,
				attachedto_info,
				args.parent_element
				->nesting_level+
				args.extra_nesting_level}});

	popup_handler->set_window_type(args.popup_window_type);

	ptr<layoutmanagerObj::implObj> popup_listlayoutmanagerptr;

	peephole_style popup_peephole_style{halign::fill};

	// Create the popup's peephole, in case the popup is too big for the
	// screen.
	auto popup_toplevel_layoutmanager=create_peephole_toplevel
		(popup_handler,
		 args.popup_border,
		 args.topleft_color,
		 args.list_style.background_color,
		 popup_peephole_style,
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

	auto p=ref<peepholed_toplevel_listcontainer_popupObj>
		::create(popup_impl,
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
