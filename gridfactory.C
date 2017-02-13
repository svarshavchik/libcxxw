/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gridfactory.H"
#include "x/w/factory.H"
#include "gridlayoutmanager.H"
#include "child_element.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

gridfactoryObj::gridfactoryObj(const gridlayoutmanager &gridlayout,
			       const ref<implObj> &impl)
	: factoryObj(gridlayout->impl->container_impl),
	  gridlayout(gridlayout), impl(impl)
{
}

gridfactoryObj::~gridfactoryObj()=default;

// Save new element's borders somewhere safe...

gridfactoryObj &gridfactoryObj::border(const border_infomm &info)
{
	auto border_impl=gridlayout->impl->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=lock->right_border=lock->top_border=
		lock->bottom_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::left_border(const border_infomm &info)
{
	auto border_impl=gridlayout->impl->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::right_border(const border_infomm &info)
{
	auto border_impl=gridlayout->impl->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->right_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::top_border(const border_infomm &info)
{
	auto border_impl=gridlayout->impl->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->top_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::bottom_border(const border_infomm &info)
{
	auto border_impl=gridlayout->impl->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->bottom_border=border_impl;
	return *this;
}

void gridfactoryObj::created(const element &new_element)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	gridlayout->impl->insert(gridlayout->lock,
				 new_element, *lock);
}

LIBCXXW_NAMESPACE_END
