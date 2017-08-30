/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "child_element.H"
#include "gridlayoutmanager.H"
#include "gridfactory.H"
#include "grid_map_info.H"
#include "x/w/element.H"
#include "gridfactory.H"
#include "metrics_grid_pos.H"
#include "current_border_impl.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

new_gridlayoutmanager::new_gridlayoutmanager()=default;

new_gridlayoutmanager::~new_gridlayoutmanager()=default;

ref<layoutmanagerObj::implObj>
new_gridlayoutmanager::create(const ref<containerObj::implObj> &parent) const
{
	return ref<gridlayoutmanagerObj::implObj>::create(parent);
}

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), lock(impl->grid_map), impl(impl)
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

gridfactory gridlayoutmanagerObj::append_row()
{
	return impl->append_row(this);
}

gridfactory gridlayoutmanagerObj::insert_row(size_t row)
{
	return impl->insert_row(this, row);
}

gridfactory gridlayoutmanagerObj::replace_row(size_t row)
{
	return impl->replace_row(this, row);
}

gridfactory gridlayoutmanagerObj::append_columns(size_t row)
{
	return impl->append_columns(this, row);
}

gridfactory gridlayoutmanagerObj::insert_columns(size_t row, size_t col)
{
	return impl->insert_columns(this, row, col);
}

void gridlayoutmanagerObj::remove()
{
	grid_map_t::lock lock{impl->grid_map};

	impl->remove_all_rows(lock);
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
	impl->remove_row(row);
}

size_t gridlayoutmanagerObj::rows()
{
	return impl->rows();
}

size_t gridlayoutmanagerObj::cols(size_t row)
{
	return impl->cols(row);
}

elementptr gridlayoutmanagerObj::get(size_t row, size_t col) const
{
	return impl->get(row, col);
}

std::optional<std::tuple<size_t, size_t>>
gridlayoutmanagerObj::lookup_row_col(grid_map_t::lock &l, const element &e)
{
	return implObj::lookup_row_col(l, e);
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
					      const std
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

void gridlayoutmanagerObj::row_alignment(size_t row, valign alignment)
{
	impl->row_alignment(row, alignment);
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
					      const std
					      ::string_view &n)
{
	auto border_impl=impl->get_theme_border(n);

	grid_map_t::lock lock{impl->grid_map};

	(*lock)->column_defaults[col].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::requested_col_width(size_t col, int percentage)
{
	impl->requested_col_width(col, percentage);
}

void gridlayoutmanagerObj::col_alignment(size_t col, halign alignment)
{
	impl->col_alignment(col, alignment);
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
