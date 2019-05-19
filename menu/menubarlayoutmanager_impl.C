/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutstyle_implfwd.H"
#include "listlayoutmanager/list_element_impl.H"
#include "listlayoutmanager/listlayoutmanager_impl.H"
#include "menu/menubarlayoutmanager_impl.H"
#include "menu/menubar_container_impl.H"
#include "menu/menubar_hotspot_implobj.H"
#include "menu/menu_impl.H"
#include "menu/menu_popup.H"
#include "x/w/impl/container.H"
#include "grid_map_info.H"
#include "x/w/gridfactory.H"
#include "x/w/canvas.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/menubarfactoryobj.H"
#include "x/w/main_window_appearance.H"
#include "x/w/focus_border_appearance.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/background_color.H"
#include "focusable_owner_container.H"
#include "generic_window_handler.H"
#include "shared_handler_data.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include <x/visitor.H>
#include <x/mp.H>

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
menupopup_delay(LIBCXX_NAMESPACE_STR "::w::menupopup_delay", 500);

menubarlayoutmanagerObj::implObj::implObj(const ref<menubar_container_implObj>
					  &container_impl,
					  const const_main_window_appearance
					  &appearance)
	: gridlayoutmanagerObj::implObj{container_impl, {}},
	  container_impl{container_impl},
	  appearance{appearance}
{
}

menubarlayoutmanagerObj::implObj::~implObj()=default;

void menubarlayoutmanagerObj::implObj::check_if_borders_changed()
{
	grid_map_t::lock grid_lock{grid_map};

	// We ignore the divider element for the purpose of this calculation.

	bool should_be_present=(*grid_lock)->elements.at(0).size() > 1;

	if (info(grid_lock).borders_present == should_be_present)
		return;

	if (should_be_present != info(grid_lock).borders_present)
	{
		if (should_be_present)
			default_row_border(grid_lock, 1,
					   appearance->menubar_border);
		else
			(*grid_lock)->remove_all_defaults();
	}
	info(grid_lock).borders_present=should_be_present;
}

layoutmanager menubarlayoutmanagerObj::implObj::create_public_object()
{
	return menubarlayoutmanager::create(ref<implObj>(this));
}

void menubarlayoutmanagerObj::implObj::initialize(menubarlayoutmanagerObj
						  *public_object,
						  menubar_lock &lock)
{
	auto f=append_row(public_object);

	row_alignment(public_object->grid_lock, 0, valign::middle);
	f->padding(0);
	f->create_canvas();
}

menu menubarlayoutmanagerObj::implObj
::add(menubarlayoutmanagerObj *public_object,
      const gridfactory &factory,
      const function<menubarfactoryObj::menu_creator_t> &creator,
      const function<menubarfactoryObj::menu_content_creator_t>
      &content_creator,
      const const_popup_list_appearance &new_popup_list_appearance,
      menubar_lock &lock)
{
	// Start by creating the popup first.

	auto &e=container_impl->get_element_impl();

	auto [menu_popup, popup_handler]=
		create_menu_popup(ref{&e}, [&]
				  (const auto &l)
				  {
					  content_creator(l);
				  },
				  new_popup_list_appearance,
				  topmenu_popup);

	auto menu_impl=ref<menuObj::implObj>
		::create(menu_popup,
			 appearance->menu_focus_border,
			 container_impl);

	auto hotspot_impl=ref<menubar_hotspot_implObj>
		::create(menu_popup,
			 popup_handler,
			 appearance->menu_highlighted_color,
			 appearance->menu_clicked_color,
			 menu_impl);

	auto hotspot=focusable_owner_container::create(hotspot_impl,
						       new_gridlayoutmanager{}
						       .create(hotspot_impl),
						       hotspot_impl);

	auto ff_impl=ref<focusframelayoutimplObj>
		::create(menu_impl, menu_impl, hotspot);

	gridlayoutmanager glm=hotspot->get_layoutmanager();

	auto creator_factory=glm->append_row();
	creator_factory->halign(halign::center);
	creator_factory->valign(valign::fill);
	creator_factory->padding(0);
	creator(glm->append_row());

	hotspot->show_all();

	auto new_menu=menu::create(menu_impl, ff_impl);

	new_menu->show();
	factory->padding(0);
	factory->created_internally(new_menu);

	new_menu->elementObj::impl->THREAD
		->run_as
		([new_menu, container_impl=this->container_impl]
		 (ONLY IN_THREAD)
		 {
			 container_impl->fix_order(IN_THREAD, new_menu);
		 });

	return new_menu;
}

void menubarlayoutmanagerObj::implObj::fix_order(ONLY IN_THREAD,
						 const menu &new_element)
{
	if (new_element->impl->tabbing_order_set(IN_THREAD))
		return; // Already did this one.

	// We must make sure the tabbing order for the menu bar's items
	// remains sane -- that the menu bars are always tabbed to first, then
	// whatever else is in the window. peepholed_toplevel_mainwindow
	// overrides get_focus_first() and makes sure that other focusable
	// elements won't get ahead of us.

	grid_map_t::lock grid_lock{grid_map};

	const auto &lookup=(*grid_lock)->get_lookup_table();

	auto found=lookup.find(new_element->impl);

	if (found == lookup.end())
		return;

	auto col= found->second->col;

	auto divider_pos=info(grid_lock).divider_pos;

	if (col > 0)
	{
		if (--col != divider_pos || col--)
		{
			menu previous=(*grid_lock)->get(0, col);

			// It won't do us any good if the previous menu item's
			// tabbing order is not set correctly yet, so make
			// sure that it is, first.

			fix_order(IN_THREAD, previous);

			new_element->get_impl()
				->get_focus_after(IN_THREAD,
						  previous->get_impl());
			return;
		}
	}

	// This must be first menu bar item.

	new_element->elementObj::impl
		->get_window_handler().get_focus_first(IN_THREAD,
						       new_element);
}

LIBCXXW_NAMESPACE_END
