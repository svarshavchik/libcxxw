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

gridfactory gridlayoutmanagerObj::insert_row(size_t row)
{
	auto me=gridlayoutmanager(this);

	{
		grid_map_t::lock lock(me->impl->grid_map);

		if ((*lock)->elements.size() < row)
			throw EXCEPTION(_("Attempting to insert a row before a nonexistent row"));

		(*lock)->elements.emplace((*lock)->elements.begin()+row);
	}

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

gridfactory gridlayoutmanagerObj::insert_columns(size_t row, size_t col)
{
	auto me=gridlayoutmanager(this);

	size_t s=({
			grid_map_t::lock lock(me->impl->grid_map);

			if ((*lock)->elements.size() <= row)
				throw EXCEPTION(_("Attempting to add columns to a nonexistent row"));

			(*lock)->elements.at(row).size();
		});

	if (col >= s)
	    throw EXCEPTION(_("Attempting to insert columns before a nonexistent column"));

	return gridfactory::create(me,
				   ref<gridfactoryObj::implObj>::create(me,
									row,
									col));
}

void gridlayoutmanagerObj::remove()
{
	grid_map_t::lock lock{impl->grid_map};

	(*lock)->elements.clear();
	(*lock)->elements_have_been_modified();
}

void gridlayoutmanagerObj::remove(size_t row, size_t col)
{
	grid_map_t::lock lock{impl->grid_map};

	if (row < (*lock)->elements.size())
	{
		auto &r=(*lock)->elements.at(row);

		if (col < r.size())
		{
			r.erase(r.begin()+col);
			(*lock)->elements_have_been_modified();
		}
	}
}

void gridlayoutmanagerObj::remove_row(size_t row)
{
	grid_map_t::lock lock{impl->grid_map};

	if (row < (*lock)->elements.size())
	{
		(*lock)->elements.erase( (*lock)->elements.begin()+row);
		(*lock)->elements_have_been_modified();
	}
}

size_t gridlayoutmanagerObj::rows()
{
	grid_map_t::lock lock{impl->grid_map};

	return (*lock)->elements.size();
}

size_t gridlayoutmanagerObj::cols(size_t row)
{
	grid_map_t::lock lock{impl->grid_map};

	if (row >= (*lock)->elements.size())
		throw EXCEPTION(_("Attempting to get the number of columns in a nonexistent row"));

	return (*lock)->elements.at(row).size();
}

elementptr gridlayoutmanagerObj::get(size_t row, size_t col) const
{
	return impl->get(row, col);
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

void gridlayoutmanagerObj::requested_row_height(size_t row, int percentage)
{
	if (percentage < 0)
		percentage=0;

	if (percentage > 100)
		percentage=100;

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->row_defaults[row].axis_size=percentage;
	(*lock)->padding_recalculated();
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

void gridlayoutmanagerObj::requested_col_width(size_t col, int percentage)
{
	if (percentage < 0)
		percentage=0;

	if (percentage > 100)
		percentage=100;

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->column_defaults[col].axis_size=percentage;
	(*lock)->padding_recalculated();
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
