/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panefactory_impl.H"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "panelayoutmanager/pane_peephole_container.H"
#include <optional>
#include "x/w/panelayoutmanager.H"
#include "x/w/panefactory.H"

LIBCXXW_NAMESPACE_START

panefactory_implObj::panefactory_implObj(const panelayoutmanager &layout)
	: panefactoryObj{layout}
{
}

panefactory_implObj::~panefactory_implObj()=default;

void panefactory_implObj::set_initial_size_set(const dim_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->dimension=arg;
}

void panefactory_implObj::set_background_color_set(const color_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->background_color=arg;
}

void panefactory_implObj::set_scrollbar_visibility_set(scrollbar_visibility v)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->pane_scrollbar_visibility=v;
}

void panefactory_implObj::left_padding_set(const dim_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->left_padding_set=arg;
}

void panefactory_implObj::right_padding_set(const dim_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->right_padding_set=arg;
}

void panefactory_implObj::top_padding_set(const dim_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->top_padding_set=arg;
}

void panefactory_implObj::bottom_padding_set(const dim_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->bottom_padding_set=arg;
}

void panefactory_implObj::padding_set(const dim_arg &arg)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->left_padding_set=arg;
	lock->right_padding_set=arg;
	lock->top_padding_set=arg;
	lock->bottom_padding_set=arg;
}


void panefactory_implObj::halign_set(LIBCXXW_NAMESPACE::halign h)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->horizontal_alignment=h;
}


void panefactory_implObj::valign_set(LIBCXXW_NAMESPACE::valign v)
{
	new_pane_properties_t::lock lock{new_pane_properties};

	lock->vertical_alignment=v;
}

container_impl panefactory_implObj::get_container_impl()
{
	auto info=layout->impl->create_pane_peephole(*this);
	new_pane_info=info;

	return info.peephole_impl;
}

elementObj::implObj &panefactory_implObj::get_element_impl()
{
	return layout->impl->pane_container_impl->container_element_impl();
}

container_impl panefactory_implObj::last_container_impl()
{
	new_pane_info_t::lock lock{new_pane_info};

	return lock->value().peephole_impl;
}

void panefactory_implObj::created_at(const element &e, size_t position)
{
	auto info=({
			new_pane_info_t::lock lock{new_pane_info};

			auto v=lock->value();

			lock->reset();

			v;
		});

	auto properties=({
			new_pane_properties_t::lock lock{new_pane_properties};

			auto v=*lock;

			*lock={};

			v;
		});

	created_pane_peephole=layout->impl
		->created_pane_peephole(layout,
					info,
					properties,
					*this,
					e,
					position,
					layout->grid_lock);
}

focusable_container panefactory_implObj
::do_create_focusable_container(const function<void
				(const focusable_container
				 &)> &creator,
				const new_focusable_layoutmanager
				&layout_manager)
{
	grid_map_t::lock lock{layout->impl->grid_map};
	// To protect the created_pane_peephole

	auto fc=panefactoryObj::do_create_focusable_container(creator,
							      layout_manager);

	pane_peephole_container new_pane_peephole=
		created_pane_peephole.get();

	//! Adjust the initial tabbing order.

	fc->get_focus_before(new_pane_peephole);

	//! And make sure it keeps getting updated, going forward.

	new_pane_peephole->focusable_element=fc;

	return fc;
}

LIBCXXW_NAMESPACE_END
