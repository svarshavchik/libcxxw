/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menu_popup.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "listlayoutmanager/list_element_impl.H"

LIBCXXW_NAMESPACE_START


//! A menu item selection has been made.

//! This is used as the selection_type called for the underlying
//! list. This gets called when a menu item has been selected/clicked
//! on.

static void menuitem_selected(const listlayoutmanager &lmbase,
			      size_t i,
			      const callback_trigger_t &trigger,
			      const busy &mcguffin)
{
	listlayoutmanager lm{lmbase};

	lm->impl->list_element_singleton->impl->menuitem_selected(lm,
								  i,
								  trigger,
								  mcguffin);
}

static std::tuple<popup, ref<popup_attachedto_handlerObj> >
do_create_menu_popup(const elementimpl &e,
		     const function<void (const listlayoutmanager &)> &creator,
		     const new_listlayoutmanager &style,
		     const create_peepholed_toplevel_listcontainer_popup_args
		     &popup_args)
{
	return create_peepholed_toplevel_listcontainer_popup
		(popup_args,
		 [&]
		 (const auto &peephole_container,
		  const popup_attachedto_info &attachedto_info)
		 ->create_popup_factory_ret_t
		 {
			 auto impl=ref<p_t_l_impl_t>
				 ::create(style,
					  peephole_container);

			 auto textlist_impl=ref<list_elementObj::implObj>
				 ::create(impl, style);

			 auto lm=ref<peepholed_toplevel_listcontainer_layoutmanager_implObj>
				 ::create(impl,list_element::create
					  (textlist_impl));

			 auto c=ref<p_t_l_t>::create
				 (attachedto_info,
				  impl,
				  lm->list_element_singleton->impl,
				  impl,
				  lm);

			 creator(lm->create_public_object());

			 return {c, c};
		 });
}

static auto
do_create_dropdown_menu(const elementimpl &e,
			const function<void (const listlayoutmanager &)>
			&creator,
			attached_to attached_to_how,
			const char *above_color,
			const char *below_color)
{
	new_listlayoutmanager style{menu_list};

	style.background_color="menu_popup_background_color";
	style.current_color="menu_popup_highlighted_color";
	style.highlighted_color="menu_popup_clicked_color";
	style.list_font=theme_font{"menu_font"};
	style.columns=1;

	style.selection_type=&menuitem_selected;

	return do_create_menu_popup
		(e, creator, style,
		 {
			 e, "dropdown_menu,popup_menu",
				 "menu",
				 "menu_popup_border",
				 0,
				 attached_to_how,
				 create_menu_popup,
				 style,
				 above_color,
				 below_color
				 });
}

std::tuple<popup, ref<popup_attachedto_handlerObj> >
topmenu_popup(const elementimpl &e,
	      const function<void (const listlayoutmanager &)> &creator)
{
	return do_create_dropdown_menu(e, creator,
				       attached_to::combobox_above_or_below,
				       "menu_above_background_color",
				       "menu_below_background_color");
}


std::tuple<popup, ref<popup_attachedto_handlerObj> >
submenu_popup(const elementimpl &e,
	      const function<void (const listlayoutmanager &)> &creator)
{
	return do_create_dropdown_menu(e, creator,
				       attached_to::submenu_next,
				       "menu_left_background_color",
				       "menu_right_background_color");
}

LIBCXXW_NAMESPACE_END
