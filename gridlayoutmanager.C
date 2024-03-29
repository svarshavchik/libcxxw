/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/container.H"
#include "x/w/impl/child_element.H"
#include "gridlayoutmanager.H"
#include "gridfactory.H"
#include "grid_map_info.H"
#include "x/w/synchronized_axis.H"
#include "x/w/element.H"
#include "gridfactory.H"
#include "metrics_grid_pos.H"
#include "x/w/impl/current_border_impl.H"
#include "messages.H"
#include <x/algorithm.H>

LIBCXXW_NAMESPACE_START

new_gridlayoutmanager::new_gridlayoutmanager()
	: synchronized_columns{synchronized_axis::create()}
{
}

new_gridlayoutmanager::~new_gridlayoutmanager()=default;

new_gridlayoutmanager::new_gridlayoutmanager(const new_gridlayoutmanager &)
=default;

new_gridlayoutmanager::new_gridlayoutmanager(new_gridlayoutmanager &&)
=default;

new_gridlayoutmanager &new_gridlayoutmanager
::operator=(const new_gridlayoutmanager &)=default;

new_gridlayoutmanager &new_gridlayoutmanager
::operator=(new_gridlayoutmanager &&)=default;

layout_impl new_gridlayoutmanager::create(const container_impl &parent) const
{
	return ref<gridlayoutmanagerObj::implObj>::create(parent, *this);
}

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj{impl}, impl{impl},
	  grid_lock{impl->grid_map}
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

void gridlayoutmanagerObj::set_background_color(const color_arg &arg)
{
	impl->layout_container_impl->container_element_impl()
		.set_background_color(arg);
}

void gridlayoutmanagerObj::remove_background_color()
{
	impl->layout_container_impl->container_element_impl()
		.remove_background_color();
}

gridfactory gridlayoutmanagerObj::append_row()
{
	modified=true;
	return impl->append_row(this);
}

gridfactory gridlayoutmanagerObj::insert_row(size_t row)
{
	modified=true;
	return impl->insert_row(this, row);
}

gridfactory gridlayoutmanagerObj::replace_row(size_t row)
{
	modified=true;
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

gridfactory gridlayoutmanagerObj::replace_cell(size_t row, size_t col)
{
	return impl->replace_cell(this, row, col);
}

void gridlayoutmanagerObj::resort_rows(const std::vector<size_t> &v)
{
	modified=true;

	if ((*grid_lock)->elements.size() != v.size())
		throw EXCEPTION(_("Number of rows in the grid is not the same"
				  " as the number of rows to resort"));

	(*grid_lock)->elements_have_been_modified();

	auto v_copy=v;

	sort_by(v_copy,
		[&]
		(size_t a, size_t b)
		{
			std::swap( (*grid_lock)->elements.at(a),
				   (*grid_lock)->elements.at(b));
		});
}

void gridlayoutmanagerObj::remove()
{
	modified=true;
	implObj::remove_all_rows(grid_lock);
}

void gridlayoutmanagerObj::remove(size_t row, size_t col)
{
	modified=true;
	implObj::remove(grid_lock, row, col);
}

void gridlayoutmanagerObj::remove_row(size_t row)
{
	modified=true;
	implObj::remove_row(grid_lock, row);
}

void gridlayoutmanagerObj::remove_rows(size_t row, size_t n)
{
	modified=true;
	implObj::remove_rows(grid_lock, row, n);
}

size_t gridlayoutmanagerObj::rows() const
{
	return (*grid_lock)->rows();
}

size_t gridlayoutmanagerObj::cols(size_t row) const
{
	return (*grid_lock)->cols(row);
}

elementptr gridlayoutmanagerObj::get(size_t row, size_t col) const
{
	notmodified();
	return implObj::get(grid_lock, row, col);
}

std::optional<std::tuple<size_t, size_t>>
gridlayoutmanagerObj::lookup_row_col(const element &e)
{
	notmodified();
	return implObj::lookup_row_col(grid_lock, e->impl);
}

void gridlayoutmanagerObj::default_row_border(size_t row,
					      const border_arg &arg)
{
	modified=true;
	impl->default_row_border(this, row, arg);
}

void gridlayoutmanagerObj::requested_row_height(size_t row, int percentage)
{
	modified=true;
	implObj::requested_row_height(grid_lock, row, percentage);
}

void gridlayoutmanagerObj::row_alignment(size_t row, valign alignment)
{
	modified=true;
	implObj::row_alignment(grid_lock, row, alignment);
}

void gridlayoutmanagerObj::row_top_padding(size_t row,
					   const dim_arg &padding)
{
	modified=true;
	implObj::row_top_padding_set(grid_lock, row, padding);
}

void gridlayoutmanagerObj::row_bottom_padding(size_t row,
					      const dim_arg &padding)
{
	modified=true;
	implObj::row_bottom_padding_set(grid_lock, row, padding);
}

void gridlayoutmanagerObj::default_col_border(size_t col,
					      const border_arg &arg)
{
	modified=true;
	impl->default_col_border(this, col, arg);
}

void gridlayoutmanagerObj::requested_col_width(size_t col, int percentage)
{
	modified=true;
	implObj::requested_col_width(grid_lock, col, percentage);
}

void gridlayoutmanagerObj::col_alignment(size_t col, halign alignment)
{
	modified=true;
	implObj::col_alignment(grid_lock, col, alignment);
}

void gridlayoutmanagerObj::col_left_padding(size_t col,
					    const dim_arg &padding)
{
	modified=true;
	implObj::col_left_padding_set(grid_lock, col, padding);
}

void gridlayoutmanagerObj::col_right_padding(size_t col,
					     const dim_arg &padding)
{
	modified=true;
	implObj::col_right_padding_set(grid_lock, col, padding);
}

void gridlayoutmanagerObj::remove_row_defaults(size_t row)
{
	modified=true;

	(*grid_lock)->row_defaults.erase(row);
	(*grid_lock)->defaults_changed();
}

void gridlayoutmanagerObj::remove_col_defaults(size_t col)
{
	modified=true;

	(*grid_lock)->column_defaults.erase(col);
	(*grid_lock)->defaults_changed();
}

LIBCXXW_NAMESPACE_END
