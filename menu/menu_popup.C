/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "menu/menu_popup.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "listlayoutmanager/list_element_impl.H"
#include "activated_in_thread.H"
#include <x/weakptr.H>

LIBCXXW_NAMESPACE_START


//! A menu item selection has been made.

//! This is used as the selection_type called for the underlying
//! list. This gets called when a menu item has been selected/clicked
//! on.

static const list_selection_type_cb_t menuitem_selected_type=
	[]
	(ONLY IN_THREAD,
	 const listlayoutmanager &lm,
	 size_t i,
	 const callback_trigger_t &trigger,
	 const busy &mcguffin)
{
	lm->impl->list_element_singleton->impl->menuitem_selected(IN_THREAD,
								  lm,
								  i,
								  trigger,
								  mcguffin);
};

static std::tuple<popup, ref<popup_attachedto_handlerObj> >
do_create_menu_popup(const function<void (const listlayoutmanager &)> &creator,
		     const new_listlayoutmanager &style,
		     const create_peepholed_toplevel_listcontainer_popup_args
		     &popup_args,
		     const function<create_p_t_l_handler_t> &create_handler)
{
	return create_peepholed_toplevel_listcontainer_popup
		(popup_args,
		 [&]
		 (const auto &peephole_container,
		  const popup_attachedto_info &attachedto_info)
		 ->create_popup_factory_ret_t
		 {
			 auto impl=ref<p_t_l_impl_t>
				 ::create(peephole_container);

			 auto textlist_impl=ref<list_elementObj::implObj>
				 ::create(list_element_impl_init_args
					  {
					   impl, style,
					   style.synchronized_columns
					  });

			 auto lm=ref<peepholed_toplevel_listcontainer_layoutmanager_implObj>
				 ::create(impl, impl,
					  list_element::create
					  (textlist_impl));

			 auto c=ref<p_t_l_t>::create
				 (attachedto_info,
				  impl,
				  lm->list_element_singleton->impl,
				  impl,
				  lm);

			 creator(lm->create_public_object());

			 return {c, c};
		 },
		 [&]
		 (const auto &args)
		 {
			 return create_handler(args);
		 });

}

static auto
do_create_dropdown_menu(const element_impl &e,
			const function<void (const listlayoutmanager &)>
			&creator,
			attached_to attached_to_how,
			const char *above_color,
			const char *below_color,
			const function<create_p_t_l_handler_t> &create_handler)
{
	new_listlayoutmanager style{menu_list};

	style.h_padding="menu_list_h_padding";
	style.v_padding="menu_list_v_padding";
	style.background_color="menu_popup_background_color";
	style.current_color="menu_popup_highlighted_color";
	style.highlighted_color="menu_popup_clicked_color";
	style.list_font=theme_font{"menu_font"};
	style.columns=1;
	style.selection_type=menuitem_selected_type;

	return do_create_menu_popup
		(creator, style,
		 {
			 e, "dropdown_menu,popup_menu",
				 "menu",
				 "menu_popup_border",
				 2,
				 attached_to_how,
				 menu_popup_type,
				 style,
				 above_color,
				 below_color
				 },
		 create_handler);
}

std::tuple<popup, ref<popup_attachedto_handlerObj> >
topmenu_popup(const element_impl &e,
	      const function<void (const listlayoutmanager &)> &creator)
{
	return do_create_dropdown_menu(e, creator,
				       attached_to::below_or_above,
				       "menu_above_background_color",
				       "menu_below_background_color",
				       make_function
				       <create_p_t_l_handler_t>
				       (create_p_t_l_handler));
}


std::tuple<popup, ref<popup_attachedto_handlerObj> >
submenu_popup(const element_impl &e,
	      const function<void (const listlayoutmanager &)> &creator)
{
	return do_create_dropdown_menu(e, creator,
				       attached_to::right_or_left,
				       "menu_left_background_color",
				       "menu_right_background_color",
				       make_function
				       <create_p_t_l_handler_t>
				       (create_p_t_l_handler));
}

namespace {
#if 0
}
#endif

//! Custom handler object for context menu popups.

//! Positions the context menu at the most recent pointer position before
//! the context menu becomes visible.

class LIBCXX_HIDDEN contextmenu_popup_handlerObj
	: public peepholed_toplevel_listcontainer_handlerObj {

	weakptr<element_implptr> contextmenu_element;

	typedef peepholed_toplevel_listcontainer_handlerObj superclass_t;

 public:
	contextmenu_popup_handlerObj
		(const peepholed_toplevel_listcontainer_handler_args &args,
		 const element_impl &contextmenu_element)
		: superclass_t{args}, contextmenu_element{contextmenu_element}
	{
	}

	~contextmenu_popup_handlerObj()=default;

	//! Override set_inherited_visibility_mapped

	//! Before mapping the popup, get the pointer position and make
	//! the popup visible here.

	void set_inherited_visibility_mapped(ONLY IN_THREAD) override
	{
		auto eptr=contextmenu_element.getptr();

		if (eptr)
		{
			auto &wh=eptr->get_window_handler();

			auto r=wh.elementObj::implObj
				::get_absolute_location_on_screen(IN_THREAD);

			r.x=coord_t::truncate(r.x+
					      wh.data(IN_THREAD).last_motion_x);
			r.y=coord_t::truncate(r.y+
					      wh.data(IN_THREAD).last_motion_y);
			r.width=1;
			r.height=1;

			update_attachedto_element_position(IN_THREAD, r);
		}

		superclass_t::set_inherited_visibility_mapped(IN_THREAD);
	}
};

#if 0
{
#endif
}

container
contextmenu_popup(const element_impl &e,
		  const function<void (const listlayoutmanager &)> &creator)
{
	auto ret=do_create_dropdown_menu
		(e, creator,
		 attached_to::below_or_above,
		 "menu_above_background_color",
		 "menu_below_background_color",

		 make_function<create_p_t_l_handler_t>
		 ([e]
		  (const auto &args)
		  {
			  return ref<contextmenu_popup_handlerObj>
			  ::create(args, e);
		  }));

	return std::get<0>(ret);
}

LIBCXXW_NAMESPACE_END
