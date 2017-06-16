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

listitemfactoryObj::listitemfactoryObj(const listlayoutmanager &me)
	: factoryObj(me->impl->container_impl),
	  me(me)
{
}

listitemfactoryObj::~listitemfactoryObj()=default;

void listitemfactoryObj::created(const element &e)
{
	new_list_items_t::lock lock{new_list_items};

	lock->push_back(e);

	// When we reach size() elements, we have a new list item.

	if (lock->size() < me->impl->columns)
		return;

	create_item(*lock);
	lock->clear(); // Ready for the next item
}

LIBCXXW_NAMESPACE_END
