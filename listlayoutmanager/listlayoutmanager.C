/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listitemfactoryobj.H"
#include "listlayoutmanager/listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

list_lock::list_lock(const const_listlayoutmanager &manager)
	: list_lock(manager->impl->grid_map)
{
}

list_lock::list_lock(grid_map_t &map) : grid_map_t::lock{map}
{
}

list_lock::~list_lock()=default;

listlayoutmanagerObj::listlayoutmanagerObj(const ref<implObj> &impl)
	: gridlayoutmanagerObj(impl),
	  impl(impl)
{
}

listlayoutmanagerObj::~listlayoutmanagerObj()=default;

class LIBCXX_HIDDEN list_append_item_factoryObj : public listitemfactoryObj {
 public:

	using listitemfactoryObj::listitemfactoryObj;

	gridfactory create_factory() const override
	{
		return me->append_row();
	}
};

factory listlayoutmanagerObj::append_item()
{
	return ref<list_append_item_factoryObj>
		::create(listlayoutmanager(this));
}

LIBCXXW_NAMESPACE_END
