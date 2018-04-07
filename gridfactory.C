/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "gridfactory.H"
#include "x/w/factory.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/child_element.H"
#include "current_border_impl.H"
#include "messages.H"
#include "generic_window_handler.H"
#include "x/w/pictformat.H"
#include <x/visitor.H>

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
	return layout->impl->container_impl->container_element_impl();
}

// Save new element's borders somewhere safe...

void gridfactoryObj::border_set(const border_arg &arg)
{
	auto border_impl=gridlayout->get_current_border(arg);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=lock->right_border=lock->top_border=
		lock->bottom_border=border_impl;
}

void gridfactoryObj::rounded_border_set(const border_arg &arg)
{
	if (get_element_impl().get_window_handler()
	    .drawable_pictformat->alpha_depth > 0)
	{
		padding_set(0);
		border_set(arg);
		return;
	}

	std::visit(visitor{
			[this](const std::string &s)
			{
				border_set(s + "_square");
				left_padding_set(s + "_square_padding_h");
				right_padding_set(s + "_square_padding_h");
				top_padding_set(s + "_square_padding_v");
				bottom_padding_set(s + "_square_padding_v");
			},
			[this](border_infomm border_info_cpy)
			{
				padding_set(border_info_cpy.radius);
				border_info_cpy.radius=0;
				border_set(border_info_cpy);
			}}, arg);
}

void gridfactoryObj::left_border_set(const border_arg &arg)
{
	auto border_impl=gridlayout->get_current_border(arg);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_border=border_impl;
}

void gridfactoryObj::right_border_set(const border_arg &arg)
{
	auto border_impl=gridlayout->get_current_border(arg);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->right_border=border_impl;
}

void gridfactoryObj::top_border_set(const border_arg &arg)
{
	auto border_impl=gridlayout->get_current_border(arg);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->top_border=border_impl;
}

void gridfactoryObj::bottom_border_set(const border_arg &arg)
{
	auto border_impl=gridlayout->get_current_border(arg);
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->bottom_border=border_impl;
}

void gridfactoryObj::padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_padding_set=padding;
	lock->right_padding_set=padding;
	lock->top_padding_set=padding;
	lock->bottom_padding_set=padding;
}

void gridfactoryObj::left_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->left_padding_set=padding;
}

void gridfactoryObj::right_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->right_padding_set=padding;
}

void gridfactoryObj::top_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->top_padding_set=padding;
}

void gridfactoryObj::bottom_padding_set(const dim_arg &padding)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->bottom_padding_set=padding;
}

void gridfactoryObj::colspan_set(size_t n)
{
	if (n <= 0 || n >= dim_t::value_type(dim_t::infinite()))
		throw EXCEPTION(_("Invalid colspan value"));

	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->width=n;
}

void gridfactoryObj::rowspan_set(size_t n)
{
	if (n <= 0 || n >= dim_t::value_type(dim_t::infinite()))
		throw EXCEPTION(_("Invalid rowspan value"));

	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->height=n;
}

void gridfactoryObj::halign_set(LIBCXXW_NAMESPACE::halign v)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->horizontal_alignment=v;
}

void gridfactoryObj::valign_set(LIBCXXW_NAMESPACE::valign v)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->vertical_alignment=v;
}

void gridfactoryObj::remove_when_hidden_set(bool flag)
{
	implObj::new_grid_element_t::lock lock(impl->new_grid_element);

	lock->remove_when_hidden=flag;
}

void gridfactoryObj::created(const element &new_element)
{
	implObj::new_grid_element_t::lock element_lock(impl->new_grid_element);

	gridlayout->insert(lock, new_element, *element_lock);
}

LIBCXXW_NAMESPACE_END
