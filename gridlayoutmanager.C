/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridfactory.H"
#include "grid_map_info.H"
#include "x/w/element.H"
#include "gridfactory.H"
#include "metrics_grid_pos.H"
#include "current_border_impl.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), lock(impl->grid_map), impl(impl)
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

gridfactory gridlayoutmanagerObj::append_row()
{
	auto me=gridlayoutmanager(this);

	dim_t row=({
			grid_map_t::lock lock(me->impl->grid_map);

			(*lock)->elements.push_back({});

			(*lock)->elements.size()-1;
		});

	return gridfactory::create(me,
				   ref<gridfactoryObj::implObj>::create(me,
									row,
									0));
}

gridfactory gridlayoutmanagerObj::append_columns(size_t row)
{
	auto me=gridlayoutmanager(this);

	dim_t col=({
			grid_map_t::lock lock(me->impl->grid_map);

			if ((*lock)->elements.size() <= row)
				throw EXCEPTION(_("Attempting to add columns to a nonexistent row"));

			(*lock)->elements.at(row).size();
		});

	return gridfactory::create(me,
				   ref<gridfactoryObj::implObj>::create(me,
									row,
									col));
}

void gridlayoutmanagerObj::erase()
{
	grid_map_t::lock lock{impl->grid_map};

	(*lock)->elements.clear();
	(*lock)->elements_have_been_modified();
}

void gridlayoutmanagerObj::erase(size_t x, size_t y)
{
	grid_map_t::lock lock{impl->grid_map};

	if (y < (*lock)->elements.size())
	{
		auto &row=(*lock)->elements.at(y);

		if (x < row.size())
		{
			row.erase(row.begin()+x);
			(*lock)->elements_have_been_modified();
		}
	}
}

elementptr gridlayoutmanagerObj::get(size_t x, size_t y) const
{
	return impl->get(x, y);
}

LIBCXXW_NAMESPACE_END
