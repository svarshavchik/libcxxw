/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "panelayoutmanager/panefactory_impl.H"
#include "panelayoutmanager/panelayoutmanager_impl.H"
#include "panelayoutmanager/pane_peephole_container.H"
#include <optional>
#include "x/w/panelayoutmanager.H"
#include "x/w/panefactory.H"
#include "x/w/pane_appearance.H"

LIBCXXW_NAMESPACE_START

panefactory_implObj::panefactory_implObj(const panelayoutmanager &layout)
	: panefactoryObj{layout}
{
}

panefactory_implObj::~panefactory_implObj()=default;

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
	new_pane_info_t::lock lock{new_pane_info};

	auto info=lock->value();

	lock->reset();

	layout->set_modified();

	grid_map_t::lock grid_lock{layout->impl->grid_map};

	created_pane_peephole=layout->impl
		->created_pane_peephole(layout,
					info,
					appearance,
					*this,
					e,
					position,
					grid_lock);

	appearance=pane_appearance::base::theme();
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
