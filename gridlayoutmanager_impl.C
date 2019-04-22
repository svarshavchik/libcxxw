/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridlayoutmanager_impl_elements.H"
#include "grid_map_info.H"
#include "synchronized_axis_value.H"
#include "x/w/impl/element.H"
#include "inherited_visibility_info.H"
#include "x/w/impl/container.H"
#include "metrics_grid_pos.H"
#include "messages.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/border_impl.H"
#include "screen.H"
#include "connection.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::implObj
::implObj(const container_impl &container_impl,
	  const new_gridlayoutmanager &glm)
	: layoutmanagerObj::implObj{container_impl},
	  grid_map{ref<grid_map_infoObj>::create(glm)},
	  grid_elements_thread_only{ref<elementsObj>::create(container_impl)},

	  // elementsObj inherits from synchronized_axis_valueObj

	  synchronized_columns{glm.synchronized_columns,
			       grid_elements_thread_only}
{
}

gridlayoutmanagerObj::implObj::~implObj()=default;

void gridlayoutmanagerObj::implObj::uninstalling(ONLY IN_THREAD)
{
	synchronized_columns.removed_from_container(IN_THREAD);
}

/*
** Using insert_or_assign to retrieve or initialize grid_map_column_defaults
** and grid_map_row_defaults. Helper objects to construct a new object only
** when necessary.

*/

namespace {
#if 0
}
#endif

struct new_column_defaults {

	grid_map_t::lock &grid_lock;

	operator grid_map_column_defaults() const
	{
		return grid_map_column_defaults{
			(*grid_lock)->grid_horiz_padding
				};
	}
};

struct new_row_defaults {

	grid_map_t::lock &grid_lock;

	operator grid_map_row_defaults() const
	{
		return grid_map_row_defaults{
			(*grid_lock)->grid_vert_padding
				};
	}
};
#if 0
{
#endif
}

static grid_map_column_defaults &get_column_defaults(grid_map_t::lock &lock,
						     size_t col)
{
	return (*lock)->column_defaults
		.insert_or_assign(col, new_column_defaults{lock}).first
		->second;
}

static grid_map_row_defaults &get_row_defaults(grid_map_t::lock &lock,
					       size_t col)
{
	return (*lock)->row_defaults
		.insert_or_assign(col, new_row_defaults{lock}).first
		->second;
}

void gridlayoutmanagerObj::implObj
::requested_col_width(grid_map_t::lock &grid_lock, size_t col, int percentage)
{
	if (percentage < 0)
		percentage=0;

	if (percentage > 100)
		percentage=100;

	get_column_defaults(grid_lock, col).axis_size=percentage;
	(*grid_lock)->padding_recalculated();
}

void gridlayoutmanagerObj::implObj
::requested_row_height(grid_map_t::lock &grid_lock, size_t row, int percentage)
{
	if (percentage < 0)
		percentage=0;

	if (percentage > 100)
		percentage=100;

	get_row_defaults(grid_lock, row).axis_size=percentage;
	(*grid_lock)->padding_recalculated();
}

gridfactory gridlayoutmanagerObj::implObj
::create_gridfactory(gridlayoutmanagerObj *public_object,
		     size_t row, size_t col, bool replace_existing)
{
	return gridfactory::create(layoutmanager{public_object},
				   public_object->impl,
				   ref<gridfactoryObj::implObj>::create
				   (row, col, *public_object->grid_lock,
				    public_object->impl
				    ->layout_container_impl,
				    replace_existing));
}

gridfactory gridlayoutmanagerObj::implObj
::append_row(gridlayoutmanagerObj *public_object)
{
	(*public_object->grid_lock)->elements_have_been_modified();
	(*public_object->grid_lock)->elements.push_back({});
	size_t row=(*public_object->grid_lock)->elements.size()-1;

	return create_gridfactory(public_object, row, 0);
}

gridfactory gridlayoutmanagerObj::implObj
::insert_row(gridlayoutmanagerObj *public_object, size_t row)
{
	if ((*public_object->grid_lock)->elements.size() < row)
		throw EXCEPTION(_("Attempting to insert a row before a nonexistent row"));

	(*public_object->grid_lock)->elements_have_been_modified();
	(*public_object->grid_lock)->elements
		.emplace((*public_object->grid_lock)->elements.begin()+row);

	return create_gridfactory(public_object, row, 0);
}

gridfactory gridlayoutmanagerObj::implObj::append_columns(gridlayoutmanagerObj
							  *public_object,
							  size_t row)
{
	if ((*public_object->grid_lock)->elements.size() <= row)
		throw EXCEPTION(_("Attempting to add columns to a nonexistent row"));

	size_t col=(*public_object->grid_lock)->elements.at(row).size();

	return create_gridfactory(public_object, row, col);
}

gridfactory gridlayoutmanagerObj::implObj::insert_columns(gridlayoutmanagerObj
							  *public_object,
							  size_t row,
							  size_t col)
{
	if ((*public_object->grid_lock)->elements.size() <= row)
		throw EXCEPTION(_("Attempting to add columns to a nonexistent row"));

	size_t s=(*public_object->grid_lock)->elements.at(row).size();

	if (col > s)
	    throw EXCEPTION(_("Attempting to insert columns before a nonexistent column"));

	return create_gridfactory(public_object, row, col);
}

gridfactory gridlayoutmanagerObj::implObj
::replace_cell(gridlayoutmanagerObj *public_object,
	       size_t row_number, size_t col_number)
{
	if ((*public_object->grid_lock)->elements.size() <= row_number)
		throw EXCEPTION(_("Attempting to replace a cell in a nonexistent row"));

	auto &row_at=(*public_object->grid_lock)->elements.at(row_number);

	size_t s=row_at.size();

	if (col_number >= s)
		throw EXCEPTION(_("Attempting to replace a display element that does not exist"));

	auto f=create_gridfactory(public_object, row_number, col_number, true);

	// Try to preserve the existing cell's properties.
	existing_grid_element_info &existing_element_info=*row_at[col_number];

	gridfactoryObj::implObj::new_grid_element_t::lock lock
		{f->impl->new_grid_element};

	existing_grid_element_info &new_existing_element_info=*lock;

	new_existing_element_info=existing_element_info;
	return f;
}

void gridlayoutmanagerObj::implObj::row_alignment(grid_map_t::lock &grid_lock,
						  size_t row, valign alignment)
{
	get_row_defaults(grid_lock, row).vertical_alignment=alignment;
}

void gridlayoutmanagerObj::implObj::col_alignment(grid_map_t::lock &grid_lock,
						  size_t col, halign alignment)
{
	get_column_defaults(grid_lock, col).horizontal_alignment=alignment;
}

void gridlayoutmanagerObj::implObj::row_top_padding_set(size_t row,
							const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	get_row_defaults(lock, row).top_padding_set=padding;
}

void gridlayoutmanagerObj::implObj::row_bottom_padding_set(size_t row,
							   const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	get_row_defaults(lock, row).bottom_padding_set=padding;
}

void gridlayoutmanagerObj::implObj::col_left_padding_set(size_t col,
							 const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	get_column_defaults(lock, col).left_padding_set=padding;
}

void gridlayoutmanagerObj::implObj::col_right_padding_set(size_t col,
							  const dim_arg &padding)
{
	grid_map_t::lock lock{grid_map};

	get_column_defaults(lock, col).right_padding_set=padding;
}

gridfactory gridlayoutmanagerObj::implObj
::replace_row(gridlayoutmanagerObj *public_object, size_t row)
{
	if ((*public_object->grid_lock)->elements.size() <= row)
		throw EXCEPTION(_("Attempting to replace a non-existent row"));

	(*public_object->grid_lock)->elements.at(row).clear();
	(*public_object->grid_lock)->elements_have_been_modified();

	return create_gridfactory(public_object, row, 0);
}

void gridlayoutmanagerObj::implObj::remove_all_rows(grid_map_t::lock &lock)
{
	(*lock)->elements.clear();
	(*lock)->elements_have_been_modified();
	(*lock)->column_defaults.clear();
	(*lock)->row_defaults.clear();
	(*lock)->defaults_changed();
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
		size_t max_n=(*lock)->elements.size()-row;

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

	return (*lock)->get(row, col);
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

	return std::nullopt;
}

void gridlayoutmanagerObj::implObj::recalculate(ONLY IN_THREAD)
{
	grid_map_t::lock lock{grid_map};

	// Even though the current position hasn't changed, we need to
	// recalculate and reposition our display elements if we already have
	// been sized by our own container.

	auto &e=get_element_impl();

	bool should_update_position=
		e.data(IN_THREAD).current_position.width > 0 &&
		e.data(IN_THREAD).current_position.height > 0;

	if (!rebuild_elements_and_update_metrics(IN_THREAD, lock,
						 should_update_position))
		return;

	if (should_update_position)
		e.schedule_update_position_processing(IN_THREAD);

#ifdef GRIDLAYOUTMANAGER_RECALCULATE_LOG
	GRIDLAYOUTMANAGER_RECALCULATE_LOG(grid_elements(IN_THREAD));
#endif
}

bool gridlayoutmanagerObj::implObj
::rebuild_elements_and_update_metrics(ONLY IN_THREAD,
				      grid_map_t::lock &grid_lock,
				      bool already_sized)
{

	// Not all recalculation is the result of inserting or removing
	// elements. rebuild_elements() will do its work only if needed.
	bool flag=rebuild_elements(IN_THREAD, grid_lock);

	if (flag)
		initialize_new_elements(IN_THREAD, grid_lock);


#ifdef CALLING_RECALCULATE
	CALLING_RECALCULATE();
#endif

	auto [final_flag, horiz_metrics, vert_metrics]=
		grid_elements(IN_THREAD)->recalculate_metrics
		(IN_THREAD, grid_lock, synchronized_columns, flag);

	if (final_flag)
		set_element_metrics(IN_THREAD, horiz_metrics, vert_metrics);

	return final_flag;
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
	 new_grid_element_info &info,
	 bool replace_existing)
{
	if (info.width == 0 || info.height == 0)
		throw EXCEPTION(_("Width or height of a new grid element cannot be 0"));

	if (info.row >= (*lock)->elements.size())
		throw EXCEPTION(_("Attempting to add a display element into a nonexistent row"));

	auto &row=(*lock)->elements.at(dim_t::value_type(info.row));

	size_t s=row.size();

	if (replace_existing && info.col >= s)
		throw EXCEPTION(_("Attempting to replace a display element that does not exist"));

	if (info.col > s)
		throw EXCEPTION(_("Attempting to add a display element into a nonexistent column"));
	auto elem=grid_element::create(info, new_element);

	if (replace_existing)
		row[dim_t::value_type(info.col)]=elem;
	else
	{
		if (info.col >= s)
			row.push_back(elem);
		else
			row.insert(row.begin()+dim_t::value_type(info.col),
				   elem);

		// Reset the next element to defaults.
		info=new_grid_element_info{info.row, info.col+1,
					   layout_container_impl,
					   *lock};
	}
	(*lock)->elements_have_been_modified();
}

void gridlayoutmanagerObj::implObj
::default_row_border(grid_map_t::lock &lock,
		     size_t row, const border_arg &arg)
{
	auto border_impl=get_current_border(arg);

	get_row_defaults(lock, row).default_border=border_impl;
	(*lock)->borders_changed();
}

void gridlayoutmanagerObj::implObj
::default_col_border(grid_map_t::lock &grid_lock,
		     size_t col, const border_arg &arg)
{
	auto border_impl=get_current_border(arg);

	get_column_defaults(grid_lock, col).default_border=border_impl;
	(*grid_lock)->borders_changed();
}

current_border_impl gridlayoutmanagerObj::implObj
::get_current_border(const border_arg &arg)
{
	return layout_container_impl->get_window_handler().screenref
		->impl->get_cached_border(arg);
}

void gridlayoutmanagerObj::implObj
::child_background_color_changed(ONLY IN_THREAD,
				 const element_impl &child)
{
	redraw_child_borders_and_padding(IN_THREAD, child);
}

///////////////////////////////////////////////////////////////////////////////
//
//
void gridlayoutmanagerObj::implObj
::requested_child_visibility_changed(ONLY IN_THREAD,
				     const element_impl &child,
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
				     const element_impl &child,
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
