/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listitemfactory.H"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listlayoutstyle.H"
#include "listlayoutmanager/listcontainer_impl.H"
#include "x/w/gridfactory.H"
#include "container.H"
#include "container_element.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

listitemfactoryObj
::listitemfactoryObj(const listlayoutmanager &me)
	: factoryObj(me->impl->container_impl),
	  me(me)
{
}

listitemfactoryObj::~listitemfactoryObj()=default;

void listitemfactoryObj::created(const element &e)
{
	new_list_items_t::lock lock{new_list_items};

	lock->elements.push_back(e);

	// When we reach size() elements, we have a new list item.

	if (lock->elements.size() < me->impl->columns)
		return;

	{
		listlayoutmanagerObj::callback_factory_container_t::lock
			factory_lock{me->callback_factory_container};

		if (*factory_lock)
			lock->status_change_callback=(*factory_lock)();
	}
	create_item(*lock);
	lock->elements.clear(); // Ready for the next item
}

LIBCXXW_NAMESPACE_END
