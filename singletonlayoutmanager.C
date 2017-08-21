/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "singletonlayoutmanager.H"
#include "singletonlayoutmanager_impl.H"
#include "x/w/factory.H"

LIBCXXW_NAMESPACE_START

singletonlayoutmanagerObj::singletonlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), impl(impl)
{
}

singletonlayoutmanagerObj::~singletonlayoutmanagerObj()=default;

class LIBCXX_HIDDEN replace_singleton_factoryObj : public factoryObj {


 public:
	const singletonlayoutmanager layout_manager;

	replace_singleton_factoryObj(const singletonlayoutmanager
				     &layout_manager)
		: factoryObj(layout_manager->impl->container_impl),
		layout_manager(layout_manager)
		{
		}

	void created(const element &e) override
	{
		singletonlayoutmanagerObj::implObj::current_element_t::lock
			lock{layout_manager->impl->current_element};

		lock->push_back(e);
	}
};

factory singletonlayoutmanagerObj::replace()
{
	return ref<replace_singleton_factoryObj>
		::create(singletonlayoutmanager(this));
}

LIBCXXW_NAMESPACE_END
