/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "singletonlayoutmanager.H"
#include "singletonlayoutmanager_impl.H"
#include "layoutmanager.H"
#include "x/w/impl/container.H"
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
		: layout_manager(layout_manager)
		{
		}

	void created(const element &e) override
	{
		layout_manager->impl->created(e);
	}

	//! Return the container for a new element.

	container_impl get_container_impl() override
	{
		return layout_manager->impl->layout_container_impl;
	}

	//! Return the container's element.

	elementObj::implObj &get_element_impl() override
	{
		return layout_manager->impl->get_element_impl();
	}

};

factory singletonlayoutmanagerObj::replace()
{
	return ref<replace_singleton_factoryObj>
		::create(singletonlayoutmanager(this));
}

LIBCXXW_NAMESPACE_END
