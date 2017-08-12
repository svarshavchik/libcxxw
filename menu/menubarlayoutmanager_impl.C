/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "menu/menubarlayoutmanager_impl.H"
#include "menu/menubar_container_impl.H"
#include "menu/menubar_hotspot_implobj.H"
#include "menu/menu_impl.H"
#include "menu/menu_layoutmanager_impl.H"
#include "peepholed_toplevel_listcontainer/create_popup.H"
#include "container.H"
#include "grid_map_info.H"
#include "x/w/gridfactory.H"
#include "x/w/canvas.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/menubarfactoryobj.H"
#include "focus/focusframelayoutimpl.H"
#include "background_color.H"
#include "focusable_owner_container.H"
#include "generic_window_handler.H"
#include "run_as.H"

LIBCXXW_NAMESPACE_START

menubarlayoutmanagerObj::implObj::implObj(const ref<menubar_container_implObj>
					  &container_impl)
	: gridlayoutmanagerObj::implObj(container_impl),
	container_impl(container_impl)
{
}

menubarlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager menubarlayoutmanagerObj::implObj::create_public_object()
{
	return menubarlayoutmanager::create(ref<implObj>(this));
}

void menubarlayoutmanagerObj::implObj::initialize(menubarlayoutmanagerObj
						  *public_object,
						  menubar_lock &lock)
{
	auto f=append_row(public_object);

	row_alignment(0, valign::middle);
	f->create_canvas();
}

menu menubarlayoutmanagerObj::implObj
::add(menubarlayoutmanagerObj *public_object,
      const gridfactory &factory,
      const function<menubarfactoryObj::menu_creator_t> &creator,
      menubar_lock &lock)
{
	// Start by creating the popup first.

	new_listlayoutmanager style{bulleted_list};

	style.background_color="menu_background_color";
	style.selected_color="menu_clicked_color";
	style.highlighted_color="menu_highlighted_color";

	auto &e=container_impl->get_element_impl();

	auto [menu_popup, popup_handler]=
		create_peepholed_toplevel_listcontainer_popup
		({
			ref<elementObj::implObj>(&e),
				"dropdown_menu,popup_menu",
				"menu_inputfocusoff_border",
				1,
				attached_to::combobox_above_or_below,
				style},
			[&]
			(const auto &peephole_container)
			{
				auto impl=ref<p_t_l_impl_t>
					::create(peephole_container, style);

				return create_p_t_l_impl_ret_t{impl,
						ref<menu_layoutmanager_implObj>
						::create(impl, impl, style)
						};
			},
			[&]
			(const popup_attachedto_info &attachedto_info,
			 const ref<p_t_l_impl_t> &impl,
			 const ref<listlayoutmanagerObj::implObj> &layout_impl)
			{
				return ref<p_t_l_t>::create(attachedto_info,
							    impl, impl,
							    layout_impl);
			});

	auto menu_impl=ref<menuObj::implObj>::create(popup_handler,
						     container_impl);

	auto ff_impl=ref<focusframelayoutimplObj>
		::create(menu_impl,
			 "menu_inputfocusoff_border",
			 "menu_inputfocuson_border");

	auto ff_factory=ff_impl->create_gridlayoutmanager()->append_row();
	ff_factory->padding(0);
	auto hotspot_impl=ref<menubar_hotspot_implObj>
		::create(menu_popup,
			 e.create_background_color("menu_background_color"),
			 e.create_background_color("menu_highlighted_color"),
			 e.create_background_color("menu_clicked_color"),
			 menu_impl);

	auto hotspot=focusable_owner_container::create(hotspot_impl,
						       ref<gridlayoutmanagerObj
						       ::implObj>::create
						       (hotspot_impl),
						       hotspot_impl);

	x::w::gridlayoutmanager glm=hotspot->get_layoutmanager();

	auto creator_factory=glm->append_row();
	creator_factory->halign(halign::center);
	creator_factory->valign(valign::fill);
	creator_factory->padding(0);
	creator(glm->append_row());

	hotspot->show_all();

	ff_factory->created_internally(hotspot);

	auto new_menu=menu::create(menu_impl, ff_impl);

	new_menu->show();
	factory->created_internally(new_menu);

	new_menu->elementObj::impl->THREAD
		->run_as
		([new_menu, container_impl=this->container_impl]
		 (IN_THREAD_ONLY)
		 {
			 container_impl->fix_order(IN_THREAD, new_menu);
		 });

	return new_menu;
}

void menubarlayoutmanagerObj::implObj::fix_order(IN_THREAD_ONLY,
						 const menu &new_element)
{
	if (new_element->impl->tabbing_order_set(IN_THREAD))
		return; // Already did this one.

	grid_map_t::lock lock{grid_map};

	const auto &lookup=(*lock)->get_lookup_table();

	auto found=lookup.find(new_element->impl);

	if (found == lookup.end())
		return;

	auto col= found->second->col;

	auto divider_pos=info(lock).divider_pos;

	if (col > 0)
	{
		if (--col != divider_pos || col--)
		{
			menu previous=get(0, col);

			// Make sure this one's tabbing order is set.

			fix_order(IN_THREAD, previous);

			new_element->get_impl()
				->get_focus_after(IN_THREAD,
						  previous->get_impl());
			return;
		}
	}

	new_element->elementObj::impl
		->get_window_handler().get_focus_first(IN_THREAD,
						       new_element);
}

LIBCXXW_NAMESPACE_END
