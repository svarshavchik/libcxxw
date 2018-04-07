/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridlayoutmanager_impl_elements.H"
#include "grid_map_info.H"
#include "x/w/impl/element.H"
#include "inherited_visibility_info.H"
#include "x/w/impl/container.H"
#include "metrics_grid_pos.H"
#include "messages.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "screen.H"
#include "connection.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: layoutmanagerObj::implObj(container_impl),
	grid_map(ref<grid_map_infoObj>::create()),
	grid_elements_thread_only(ref<elementsObj>::create())
{
}

gridlayoutmanagerObj::implObj::~implObj()=default;

void gridlayoutmanagerObj::implObj
::requested_col_width(size_t col, int percentage)
{
	if (percentage < 0)
		percentage=0;

	if (percentage > 100)
		percentage=100;

	grid_map_t::lock lock{grid_map};

	(*lock)->column_defaults[col].axis_size=percentage;
	(*lock)->padding_recalculated();
}

void gridlayoutmanagerObj::implObj
::requested_row_height(size_t row, int percentage)
{
	if (percentage < 0)
		percentage=0;

	if (percentage > 100)
		percentage=100;

	grid_map_t::lock lock{grid_map};

	(*lock)->row_defaults[row].axis_size=percentage;
	(*lock)->padding_recalculated();
}

gridfactory gridlayoutmanagerObj::implObj
::create_gridfactory(layoutmanagerObj *public_object,
		     size_t row, size_t col)
{
	grid_map_t::lock lock{grid_map};

	return gridfactory::create(layoutmanager{public_object},
				   ref<implObj>(this),
				   ref<gridfactoryObj::implObj>::create
				   (row, col, *lock));
}

gridfactory gridlayoutmanagerObj::implObj
::append_row(layoutmanagerObj *public_object)
{
	size_t row=({
			grid_map_t::lock lock(grid_map);

			(*lock)->elements.push_back({});

			(*lock)->elements.size()-1;
		});

	return create_gridfactory(public_object, row, 0);
}

gridfactory gridlayoutmanagerObj::implObj
::insert_row(layoutmanagerObj *public_object, size_t row)
{
	{
		grid_map_t::lock lock(grid_map);

		if ((*lock)->elements.size() < row)
			throw EXCEPTION(_("Attempting to insert a row before a nonexistent row"));

		(*lock)->elements_have_been_modified();
		(*lock)->elements.emplace((*lock)->elements.begin()+row);
	}

	return create_gridfactory(public_object, row, 0);
}

gridfactory gridlayoutmanagerObj::implObj::append_columns(layoutmanagerObj
							  *public_object,
							  size_t row)
{
	grid_map_t::lock lock(grid_map);

	if ((*lock)->elements.size() <= row)
		throw EXCEPTION(_("Attempting to add columns to a nonexistent row"));

	size_t col=(*lock)->elements.at(row).size();

	return create_gridfactory(public_object, row, col);
}

gridfactory gridlayoutmanagerObj::implObj::insert_columns(layoutmanagerObj
							  *public_object,
							  size_t row,
							  size_t col)
{
	grid_map_t::lock lock(grid_map);

	if ((*lock)->elements.size() <= row)
		throw EXCEPTION(_("Attempting to add columns to a nonexistent row"));

	size_t s=(*lock)->elements.at(row).size();

	if (col > s)
	    throw EXCEPTION(_("Attempting to insert columns before a nonexistent column"));

	return create_gridfactory(public_object, row, col);
}

size_t gridlayoutmanagerObj::implObj::rows()
{
	grid_map_t::lock lock{grid_map};

	return (*lock)->elements.size();
}

size_t gridlayoutmanagerObj::implObj::cols(size_t row)
{
	grid_map_t::lock lock{grid_map};

	if (row >= (*lock)->elements.size())
		throw EXCEPTION(_("Attempting to get the number of columns in a nonexistent row"));

	return (*lock)->elements.at(row).size();
}

void gridlayoutmanagerObj::implObj::row_alignment(size_t row, valign alignment)
{
	grid_map_t::lock lock{grid_map};

	(*lock)->row_defaults[row].vertical_alignment=alignment;
}

void gridlayoutmanagerObj::implObj::col_alignment(size_t col, halign alignment)
{
	grid_map_t::lock lock{grid_map};

	(*lock)->column_defaults[col].horizontal_alignment=alignment;
}

void gridlayoutmanagerObj::implObj::row_top_padding_set(size_t row,
							const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	(*lock)->row_defaults[row].top_padding_set=padding;
}

void gridlayoutmanagerObj::implObj::row_bottom_padding_set(size_t row,
							   const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	(*lock)->row_defaults[row].bottom_padding_set=padding;
}

void gridlayoutmanagerObj::implObj::col_left_padding_set(size_t col,
							 const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	(*lock)->column_defaults[col].left_padding_set=padding;
}

void gridlayoutmanagerObj::implObj::col_right_padding_set(size_t col,
							  const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	(*lock)->column_defaults[col].right_padding_set=padding;
}

gridfactory gridlayoutmanagerObj::implObj
::replace_row(layoutmanagerObj *public_object, size_t row)
{
	{
		grid_map_t::lock lock(grid_map);

		if ((*lock)->elements.size() <= row)
			throw EXCEPTION(_("Attempting to replace a non-existent row"));

		(*lock)->elements.at(row).clear();
		(*lock)->elements_have_been_modified();
	}

	return create_gridfactory(public_object, row, 0);
}

void gridlayoutmanagerObj::implObj::remove_all_rows(grid_map_t::lock &lock)
{
	(*lock)->elements.clear();
	(*lock)->elements_have_been_modified();
}

void gridlayoutmanagerObj::implObj::remove(grid_map_t::lock &lock,
					   size_t row,
					   size_t col,
					   size_t n)
{
	if (row < (*lock)->elements.size())
	{
		auto &r=(*lock)->elements.at(row);

		size_t s=r.size();

		if (col < s)
		{
			if (s-col < n)
				n=s-col;

			auto b=r.begin()+col;

			r.erase(b, b+n);
			(*lock)->elements_have_been_modified();
		}
	}
}

void gridlayoutmanagerObj::implObj::remove_row(size_t row)
{
	remove_rows(row, 1);
}

void gridlayoutmanagerObj::implObj::remove_rows(size_t row, size_t n)
{
	grid_map_t::lock lock{grid_map};

	if (row < (*lock)->elements.size())
	{
		size_t max_n=(*lock)->elements.size()-n;

		if (max_n < n)
			n=max_n;

		auto b=(*lock)->elements.begin()+row;

		(*lock)->elements.erase(b, b+n);
		(*lock)->elements_have_been_modified();
	}
}

elementptr gridlayoutmanagerObj::implObj::get(size_t row, size_t col)
{
	grid_map_t::lock lock{grid_map};

	if (row < (*lock)->elements.size())
	{
		const auto &r=(*lock)->elements.at(row);

		if (col < r.size())
			return r.at(col)->grid_element;
	}
	return elementptr();
}

std::optional<std::tuple<size_t, size_t>>
gridlayoutmanagerObj::implObj::lookup_row_col(const ref<elementObj::implObj> &e)
{
	grid_map_t::lock lock{grid_map};

	return lookup_row_col(lock, e);
}

std::optional<std::tuple<size_t, size_t>>
gridlayoutmanagerObj::implObj::lookup_row_col(grid_map_t::lock &lock,
					      const ref<elementObj::implObj> &e)
{
	const auto &lookup_table=(*lock)->get_lookup_table();

	auto iter=lookup_table.find(e);

	if (iter != lookup_table.end())
		return std::tuple{iter->second->row, iter->second->col};

	return {};
}

void gridlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	// Not all recalculation is the result of inserting or removing
	// elements. rebuild_elements() will do its work only if needed.
	bool flag=rebuild_elements(IN_THREAD);

	if (flag)
		initialize_new_elements(IN_THREAD);


#ifdef CALLING_RECALCULATE
	CALLING_RECALCULATE();
#endif

	auto [final_flag, horiz_metrics, vert_metrics]=
		grid_elements(IN_THREAD)->recalculate_metrics(IN_THREAD, flag);

	if (!final_flag)
		return;

	set_element_metrics(IN_THREAD, horiz_metrics, vert_metrics);

	// Even though the current position hasn't changed, we need to
	// recalculate and reposition our display elements.
	get_element_impl().schedule_update_position_processing(IN_THREAD);

#ifdef GRIDLAYOUTMANAGER_RECALCULATE_LOG
	GRIDLAYOUTMANAGER_RECALCULATE_LOG(grid_elements(IN_THREAD));
#endif
}

void gridlayoutmanagerObj::implObj::set_element_metrics(ONLY IN_THREAD,
							const metrics::axis &h,
							const metrics::axis &v)
{
	get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD, h, v);
}

layoutmanager gridlayoutmanagerObj::implObj::create_public_object()
{
	return create_gridlayoutmanager();
}

gridlayoutmanager gridlayoutmanagerObj::implObj::create_gridlayoutmanager()
{
	return gridlayoutmanager::create(ref<implObj>(this));
}

void gridlayoutmanagerObj::implObj
::insert(grid_map_t::lock &lock,
	 const element &new_element,
	 new_grid_element_info &info)
{
	if (info.width == 0 || info.height == 0)
		throw EXCEPTION(_("Width or height of a new grid element cannot be 0"));

	if (info.row >= (*lock)->elements.size())
		throw EXCEPTION(_("Attempting to add a display element into a nonexistent row"));

	auto &row=(*lock)->elements.at(dim_t::value_type(info.row));

	if (info.col > row.size())
		throw EXCEPTION(_("Attempting to add a display element into a nonexistent column"));
	auto elem=grid_element::create(info, new_element);

	if (info.col >= row.size())
		row.push_back(elem);
	else
		row.insert(row.begin()+dim_t::value_type(info.col), elem);

	// Reset the next element to defaults.
	info=new_grid_element_info{info.row, info.col+1, *lock};
	(*lock)->elements_have_been_modified();
}

void gridlayoutmanagerObj::implObj
::default_row_border(size_t row, const border_arg &arg)
{
	auto border_impl=get_current_border(arg);

	grid_map_t::lock lock{grid_map};

	(*lock)->row_defaults[row].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::implObj
::default_col_border(size_t col, const border_arg &arg)
{
	auto border_impl=get_current_border(arg);

	grid_map_t::lock lock{grid_map};

	(*lock)->column_defaults[col].default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::implObj::remove_all_defaults()
{
	grid_map_t::lock lock{grid_map};

	(*lock)->row_defaults.clear();
	(*lock)->column_defaults.clear();
	(*lock)->borders_changed();
}

current_border_impl gridlayoutmanagerObj::implObj
::get_current_border(const border_arg &arg)
{
	return container_impl->get_window_handler().screenref
		->impl->get_cached_border(arg);
}

void gridlayoutmanagerObj::implObj
::child_background_color_changed(ONLY IN_THREAD,
				 const elementimpl &child)
{
	redraw_child_borders_and_padding(IN_THREAD, child);
}

///////////////////////////////////////////////////////////////////////////////
//
//
void gridlayoutmanagerObj::implObj
::requested_child_visibility_changed(ONLY IN_THREAD,
				     const elementimpl &child,
				     bool flag)
{
	grid_map_t::lock lock(grid_map);

	auto &lookup_table=(*lock)->get_lookup_table();

	auto lookup=lookup_table.find(child);

	if (lookup == lookup_table.end())
		return;

	const auto &ge=(*lock)->elements.at(lookup->second->row)
		.at(lookup->second->col);

	if (ge->remove_when_hidden)
	{
		// takes_up_space() checks requested_visibility when
		// remove_when_hidden.

		(*lock)->borders_changed();
		needs_recalculation(IN_THREAD);
	}
}

void gridlayoutmanagerObj::implObj
::inherited_child_visibility_changed(ONLY IN_THREAD,
				     const elementimpl &child,
				     inherited_visibility_info &info)
{
	grid_map_t::lock lock(grid_map);

	auto &lookup_table=(*lock)->get_lookup_table();

	auto lookup=lookup_table.find(child);

	if (lookup == lookup_table.end())
		return;

	const auto &ge=(*lock)->elements.at(lookup->second->row)
		.at(lookup->second->col);

	if (!ge->remove_when_hidden)
	{
		// The layout of the container is not going to be changed
		// as a result of the child visibility change. The only thing
		// that needs to be done is to redraw the padding around the
		// element (if there is any) to reflect the presence/absence
		// of the child element's background color, which is used for
		// the padding when the child element is visible.

		if (!info.do_not_redraw)
			redraw_child_borders_and_padding(IN_THREAD, child);
	}
}

LIBCXXW_NAMESPACE_END
