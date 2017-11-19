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
#include "messages.H"

LIBCXXW_NAMESPACE_START

gridfactoryObj::gridfactoryObj(const layoutmanager &layout,
			       const ref<gridlayoutmanagerObj::implObj> &gridlayout,
			       const ref<implObj> &impl)
	: layout(layout),
	  gridlayout(gridlayout),
	  lock(gridlayout->grid_map),
	  impl(impl)
{
}

gridfactoryObj::~gridfactoryObj()=default;

ref<containerObj::implObj> gridfactoryObj::get_container_impl()
{
	return layout->impl->container_impl;
}

elementObj::implObj &gridfactoryObj::get_element_impl()
{
	return layout->impl->container_impl->get_element_impl();
}

// Save new element's borders somewhere safe...

gridfactoryObj &gridfactoryObj::border(const border_infomm &info)
{
	auto border_impl=gridlayout->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=lock->right_border=lock->top_border=
		lock->bottom_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::left_border(const border_infomm &info)
{
	auto border_impl=gridlayout->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::right_border(const border_infomm &info)
{
	auto border_impl=gridlayout->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->right_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::top_border(const border_infomm &info)
{
	auto border_impl=gridlayout->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->top_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::bottom_border(const border_infomm &info)
{
	auto border_impl=gridlayout->get_custom_border(info);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->bottom_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::border(const std::string_view &id)
{
	auto border_impl=gridlayout->get_theme_border(id);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=lock->right_border=lock->top_border=
		lock->bottom_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::left_border(const std::string_view &id)
{
	auto border_impl=gridlayout->get_theme_border(id);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::right_border(const std::string_view &id)
{
	auto border_impl=gridlayout->get_theme_border(id);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->right_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::top_border(const std::string_view &id)
{
	auto border_impl=gridlayout->get_theme_border(id);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->top_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::bottom_border(const std::string_view &id)
{
	auto border_impl=gridlayout->get_theme_border(id);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->bottom_border=border_impl;
	return *this;
}

gridfactoryObj &gridfactoryObj::padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_padding_set=padding;
	lock->right_padding_set=padding;
	lock->top_padding_set=padding;
	lock->bottom_padding_set=padding;

	return *this;
}

gridfactoryObj &gridfactoryObj::left_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_padding_set=padding;

	return *this;
}

gridfactoryObj &gridfactoryObj::right_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->right_padding_set=padding;

	return *this;
}

gridfactoryObj &gridfactoryObj::top_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->top_padding_set=padding;

	return *this;
}

gridfactoryObj &gridfactoryObj::bottom_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->bottom_padding_set=padding;

	return *this;
}

gridfactoryObj &gridfactoryObj::colspan(size_t n)
{
	if (n <= 0 || n >= dim_t::value_type(dim_t::infinite()))
		throw EXCEPTION(_("Invalid colspan value"));

	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->width=n;
	return *this;
}

gridfactoryObj &gridfactoryObj::rowspan(size_t n)
{
	if (n <= 0 || n >= dim_t::value_type(dim_t::infinite()))
		throw EXCEPTION(_("Invalid rowspan value"));

	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->height=n;
	return *this;
}

gridfactoryObj &gridfactoryObj::halign(LIBCXXW_NAMESPACE::halign v)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->horizontal_alignment=v;
	return *this;
}

gridfactoryObj &gridfactoryObj::valign(LIBCXXW_NAMESPACE::valign v)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->vertical_alignment=v;
	return *this;
}

gridfactoryObj &gridfactoryObj::remove_when_hidden(bool flag)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->remove_when_hidden=flag;
	return *this;
}

void gridfactoryObj::created(const element &new_element)
{
	implObj::new_grid_element_t::lock element_lock(impl->new_grid_element);

	gridlayout->insert(lock, new_element, *element_lock);
}

LIBCXXW_NAMESPACE_END
