/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_style.H"
#include "peephole/peephole_toplevel.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "generic_window_handler.H"
#include "popup/popup_attachedto_handler.H"
#include "popup/popup_impl.H"
#include "listlayoutmanager/listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

//! Override get_layout_impl() in create_peepholed_toplevel_listcontainer_popup)_'s popup.

//! The popup returned by create_peepholed_toplevel_listcontainer_popup()
//! gets its get_layout_impl() overwritten to return the list layout manager
//! of the poup's list container, rather than its actual top level manager,
//! the one for the peephole.

class LIBCXX_HIDDEN peepholed_toplevel_listcontainer_popupObj
	: public popupObj {

 public:

	const ref<listlayoutmanagerObj::implObj> listlayout_impl;

	peepholed_toplevel_listcontainer_popupObj
		(const ref<implObj> &impl,
		 const ref<layoutmanagerObj::implObj> &layout,
		 const ref<listlayoutmanagerObj::implObj> &listlayout_impl)
		: popupObj(impl, layout),
		listlayout_impl(listlayout_impl)
		{
		}

	~peepholed_toplevel_listcontainer_popupObj()=default;

	ref<layoutmanagerObj::implObj> get_layout_impl() const
	{
		return listlayout_impl;
	}
};

create_p_t_l_popup_ret_t
do_create_peepholed_toplevel_listcontainer_popup
(const create_peepholed_toplevel_listcontainer_popup_args &args,
 const function<create_p_t_l_impl_t> &impl_factory,
 const function<create_p_t_l_t> &factory)
{
	// First, the handler.

	auto parent_handler=ref<generic_windowObj::handlerObj>
		(&args.parent_element->get_window_handler());

	auto attachedto_info=popup_attachedto_info::create
		(rectangle{}, args.attached_to_style);

	auto popup_handler=ref<popup_attachedto_handlerObj>
		::create(parent_handler, attachedto_info,
			 args.parent_element->nesting_level+
			 args.extra_nesting_level);

	popup_handler->set_window_type(args.popup_window_type);

	// We are not using new_listlayoutmanager::create(), we need to
	// set the handler's background color ourselves.

	popup_handler->elementObj::implObj
		::set_background_color(args.list_style.background_color);

	ptr<peepholed_toplevel_listcontainer_layoutmanager_implObj
	    > popup_listlayoutmanagerptr;

	peephole_style my_peephole_style;

	my_peephole_style.h_alignment=halign::fill;

	// Create the popup's peephole, in case the popup is too big for the
	// screen.
	auto popup_toplevel_layoutmanager=create_peephole_toplevel
		(popup_handler,
		 args.popup_border,
		 my_peephole_style,
		[&]
		 (const auto &peephole_container)
		 {
			 auto [impl, popup_listlayoutmanager]=
			 impl_factory(peephole_container);

			 auto popup=factory(attachedto_info,
					    impl,
					    popup_listlayoutmanager);

			 popup->show();
			 popup_listlayoutmanagerptr=popup_listlayoutmanager;

			 return popup;
		 });

	// Now, finish creating the popup's implementation object, and the
	// popup "public" object itself.

	auto popup_impl=ref<popupObj::implObj>::create(popup_handler,
						       parent_handler);

	return {
		ref<peepholed_toplevel_listcontainer_popupObj>
			::create(popup_impl,
				 popup_toplevel_layoutmanager->impl,
				 popup_listlayoutmanagerptr),

			popup_impl->handler
			};
}

LIBCXXW_NAMESPACE_END