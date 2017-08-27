/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "menu/menulistitemfactoryobj.H"
#include "menu/menuitemextrainfo.H"
#include "menu/menuitemextrainfo_impl.H"
#include "x/w/menulayoutmanager.H"

LIBCXXW_NAMESPACE_START

menulistitemfactoryObj::~menulistitemfactoryObj()=default;

void menulistitemfactoryObj::set_type(const menuitem_type_t &menuitem_type)
{
	next_menuitem_type_t::lock lock{next_menuitem_type};

	*lock=menuitem_type;
}

void menulistitemfactoryObj::created_element(const element &e)
{
	next_menuitem_type_t::lock lock{next_menuitem_type};

	listitemfactoryObj::created_element(e);

	// If we created all but the last element, time to insert our extra
	// info.

	{
		new_list_items_t::lock lock{new_list_items};

		if (lock->elements.size() + 1 < me->impl->columns)
			return;
	}

	auto impl=ref<menuitemextrainfoObj::implObj>::create
		(me->impl->container_impl);

	auto new_item=menuitemextrainfo::create(impl, *lock);

	new_item->update_shortcut(*lock);
	listitemfactoryObj::created_element(new_item);

	*lock=menuitem_type_t{}; // Reset for the next menu item.
}

LIBCXXW_NAMESPACE_END
