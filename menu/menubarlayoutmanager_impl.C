/*
** Copyright 2017-2018 Double Precision, Inc.
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
#include "shared_handler_data.H"
#include "run_as.H"
#include "catch_exceptions.H"
#include <x/visitor.H>
#include <x/mp.H>

LIBCXXW_NAMESPACE_START

static property::value<unsigned>
menupopup_delay(LIBCXX_NAMESPACE_STR "::w::menupopup_delay", 500);

menubarlayoutmanagerObj::implObj::implObj(const ref<menubar_container_implObj>
					  &container_impl)
	: gridlayoutmanagerObj::implObj(container_impl),
	container_impl(container_impl)
{
}

menubarlayoutmanagerObj::implObj::~implObj()=default;

void menubarlayoutmanagerObj::implObj::check_if_borders_changed()
{
	grid_map_t::lock lock{grid_map};

	// We ignore the divider element for the purpose of this calculation.

	bool should_be_present=(*lock)->elements.at(0).size() > 1;

	if (info(lock).borders_present == should_be_present)
		return;

	if (should_be_present)
		default_row_border(1, "menubar_border");
	else
		remove_all_defaults();

	info(lock).borders_present=should_be_present;
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

	row_alignment(0, valign::middle);
	f->padding(0);
	f->create_canvas();
}

menu menubarlayoutmanagerObj::implObj
::add(menubarlayoutmanagerObj *public_object,
      const gridfactory &factory,
      const function<menubarfactoryObj::menu_creator_t> &creator,
      const function<menubarfactoryObj::menu_content_creator_t>
      &content_creator,
      menubar_lock &lock)
{
	// Start by creating the popup first.

	auto &e=container_impl->get_element_impl();

	auto [menu_popup, popup_handler]=
		create_menu_popup(ref(&e), [&]
				  (const auto &l)
				  {
					  content_creator(l);
				  }, topmenu_popup);

	auto menu_impl=ref<menuObj::implObj>
		::create(menu_popup,
			 "menu_inputfocusoff_border",
			 "menu_inputfocuson_border",
			 popup_handler,
			 container_impl);

	auto ff_impl=ref<focusframelayoutimplObj>
		::create(menu_impl);

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
	factory->padding(0);
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

	// We must make sure the tabbing order for the menu bar's items
	// remains sane -- that the menu bars are always tabbed to first, then
	// whatever else is in the window. peepholed_toplevel_mainwindow
	// overrides get_focus_first() and makes sure that other focusable
	// elements won't get ahead of us.

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
