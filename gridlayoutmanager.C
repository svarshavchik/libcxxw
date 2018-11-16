/*
** Copyright 2017 Double Precision, Inc.
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

LIBCXXW_NAMESPACE_START

new_gridlayoutmanager::new_gridlayoutmanager()
	: synchronized_columns{synchronized_axis::create()}
{
}

new_gridlayoutmanager::~new_gridlayoutmanager()=default;

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

gridfactory gridlayoutmanagerObj::replace_cell(size_t row, size_t col)
{
	return impl->replace_cell(this, row, col);
}

void gridlayoutmanagerObj::remove()
{
	impl->remove_all_rows(grid_lock);
}

void gridlayoutmanagerObj::remove(size_t row, size_t col)
{
	impl->remove(grid_lock, row, col);
}

void gridlayoutmanagerObj::remove_row(size_t row)
{
	impl->remove_row(row);
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
	return (*grid_lock)->get(row, col);
}

std::optional<std::tuple<size_t, size_t>>
gridlayoutmanagerObj::lookup_row_col(const element &e)
{
	return implObj::lookup_row_col(grid_lock, e);
}

void gridlayoutmanagerObj::default_row_border(size_t row,
					      const border_arg &arg)
{
	impl->default_row_border(row, arg);
}

void gridlayoutmanagerObj::requested_row_height(size_t row, int percentage)
{
	impl->requested_row_height(grid_lock, row, percentage);
}

void gridlayoutmanagerObj::row_alignment(size_t row, valign alignment)
{
	impl->row_alignment(grid_lock, row, alignment);
}

void gridlayoutmanagerObj::row_top_padding_set(size_t row,
					       const dim_arg &padding)
{
	impl->row_top_padding_set(row, padding);
}

void gridlayoutmanagerObj::row_bottom_padding_set(size_t row,
						  const dim_arg &padding)
{
	impl->row_bottom_padding_set(row, padding);
}

void gridlayoutmanagerObj::default_col_border(size_t col,
					      const border_arg &arg)
{
	impl->default_col_border(col, arg);
}

void gridlayoutmanagerObj::requested_col_width(size_t col, int percentage)
{
	impl->requested_col_width(grid_lock, col, percentage);
}

void gridlayoutmanagerObj::col_alignment(size_t col, halign alignment)
{
	impl->col_alignment(grid_lock, col, alignment);
}

void gridlayoutmanagerObj::col_left_padding_set(size_t col,
						const dim_arg &padding)
{
	impl->col_left_padding_set(col, padding);
}

void gridlayoutmanagerObj::col_right_padding_set(size_t col,
						 const dim_arg &padding)
{
	impl->col_right_padding_set(col, padding);
}

void gridlayoutmanagerObj::remove_row_defaults(size_t row)
{
	(*grid_lock)->row_defaults.erase(row);
	(*grid_lock)->defaults_changed();
}

void gridlayoutmanagerObj::remove_col_defaults(size_t col)
{
	(*grid_lock)->column_defaults.erase(col);
	(*grid_lock)->defaults_changed();
}

LIBCXXW_NAMESPACE_END
