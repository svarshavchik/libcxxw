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

void gridlayoutmanagerObj::default_row_border(size_t row,
					      const border_infomm &info)
{
	auto border_impl=impl->get_custom_border(info);

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->row_defaults[row].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::default_row_border(size_t row,
					      const std::experimental
					      ::string_view &n)
{
	auto border_impl=impl->get_theme_border(n);

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->row_defaults[row].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::default_col_border(size_t col,
					      const border_infomm &info)
{
	auto border_impl=impl->get_custom_border(info);

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->column_defaults[col].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::default_col_border(size_t col,
					      const std::experimental
					      ::string_view &n)
{
	auto border_impl=impl->get_theme_border(n);

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->column_defaults[col].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::remove_row_defaults(size_t row)
{
	grid_map_t::lock lock{impl->grid_map};

	(*lock)->row_defaults.erase(row);
	(*lock)->defaults_changed();
}

void gridlayoutmanagerObj::remove_col_defaults(size_t col)
{
	grid_map_t::lock lock{impl->grid_map};

	(*lock)->column_defaults.erase(col);
	(*lock)->defaults_changed();
}

LIBCXXW_NAMESPACE_END
