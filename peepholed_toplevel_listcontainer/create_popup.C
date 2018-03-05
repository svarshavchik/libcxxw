/*
** Copyright 2017-2018 Double Precision, Inc.
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
#include "shared_handler_data.H"
#include "gridlayoutmanager.H"
#include "background_color.H"

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

	ref<layoutmanagerObj::implObj> get_layout_impl() const override
	{
		return listlayout_impl;
	}
};

//! Popup handler object

//! Stored the background colors used for the list's background when the
//! list popup is above and below, or to the left or the right, of the
//! attached_to element.
//!
//! Overrides recalculate_popup_position, and captures the newly-calculated
//! popup_position_affinity, that determines which background color to set
//! for the list popup's background, which gets updated accordingly.

class LIBCXX_HIDDEN listcontainer_popup_attachedto_handlerObj
	: public popup_attachedto_handlerObj {

	typedef popup_attachedto_handlerObj superclass_t;

	//! Which affinity is used to set the current list popup background

	//! do_create_peepholed_toplevel_listcontainer_popup initializes the
	//! list popup's background to the topright background color, so that's
	//! the opening bid.

	popup_position_affinity current_affinity=popup_position_affinity::above;

	//! Background color when attached to left or above

	const background_color topleft_color;

	//! Background color when attached to right or below.

	const background_color bottomright_color;

 public:

	template<typename ...Args>
		listcontainer_popup_attachedto_handlerObj
		(const color_arg &topleft_color,
		 const color_arg &bottomright_color,
		 Args && ...args)
		: superclass_t{std::forward<Args>(args)...},
		topleft_color{this->create_background_color(topleft_color)},
			bottomright_color{this->create_background_color
					(bottomright_color)}
	{
	}

	popup_position_affinity recalculate_popup_position(IN_THREAD_ONLY,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override
	{
		auto adjusted_current_affinity=current_affinity;

		current_affinity=superclass_t::recalculate_popup_position
			(IN_THREAD, r,
			 screen_width,
			 screen_height);

		auto adjusted_new_affinity=current_affinity;

		switch (adjusted_current_affinity) {
		case popup_position_affinity::left:
			adjusted_current_affinity=
				popup_position_affinity::above;
			break;

		case popup_position_affinity::right:
			adjusted_current_affinity=
				popup_position_affinity::below;
			break;
		case popup_position_affinity::above:
		case popup_position_affinity::below:
			break;
		}

		switch (adjusted_new_affinity) {
		case popup_position_affinity::left:
			adjusted_new_affinity=popup_position_affinity::above;
			break;

		case popup_position_affinity::right:
			adjusted_new_affinity=
				popup_position_affinity::below;
			break;
		case popup_position_affinity::above:
		case popup_position_affinity::below:
			break;
		}

		if (adjusted_current_affinity == adjusted_new_affinity)
			return current_affinity;

		invoke_layoutmanager
			([&]
			 (const ref<gridlayoutmanagerObj::implObj> &peephole_lm)
			 {
				 // The top level element is a grid with a
				 // peephole being element (0, 0) in the grid.

				 auto i=peephole_lm->get(0, 0)->impl;

				 auto &c=adjusted_new_affinity ==
					 popup_position_affinity::above
					 ? topleft_color
					 : bottomright_color;

				 i->set_background_color(IN_THREAD, c);
			 });

		return current_affinity;
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
 const function<create_p_t_l_impl_t> &impl_factory,
 const function<create_p_t_l_t> &factory)
{
	// First, the handler.

	auto parent_handler=ref<generic_windowObj::handlerObj>
		(&args.parent_element->get_window_handler());

	auto attachedto_info=popup_attachedto_info::create
		(rectangle{}, args.attached_to_style);

	auto [opened_popup, closed_popup]=(*args.popup_type)();

	auto popup_handler=ref<listcontainer_popup_attachedto_handlerObj>
		::create(args.topleft_color,
			 args.bottomright_color,
			 opened_popup,
			 closed_popup,
			 args.popup_wm_class_instance,
			 parent_handler,
			 "transparent",
			 attachedto_info,
			 args.parent_element->nesting_level+
			 args.extra_nesting_level);

	popup_handler->set_window_type(args.popup_window_type);

	ptr<peepholed_toplevel_listcontainer_layoutmanager_implObj
	    > popup_listlayoutmanagerptr;

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
			 auto [impl, popup_listlayoutmanager]=
			 impl_factory(peephole_container);

			 auto popup=factory(attachedto_info,
					    impl,
					    popup_listlayoutmanager);

			 popup_listlayoutmanagerptr=popup_listlayoutmanager;

			 return popup;
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

LIBCXXW_NAMESPACE_END
