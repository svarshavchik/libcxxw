/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridlayoutmanager_impl_elements.H"
#include "element.H"
#include "element_screen.H"
#include "container.H"
#include "metrics_grid_pos.H"
#include "messages.H"
#include "current_border_impl.H"
#include "border_impl.H"
#include "screen.H"
#include "connection.H"
#include "generic_window_handler.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::grid_map_info_t::grid_map_info_t()=default;

gridlayoutmanagerObj::grid_map_info_t::~grid_map_info_t()=default;

gridlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: layoutmanagerObj::implObj(container_impl),
	grid_elements_thread_only(ref<elementsObj>::create())
{
}

gridlayoutmanagerObj::implObj::~implObj()=default;

elementptr gridlayoutmanagerObj::implObj::get(size_t x, size_t y)
{
	grid_map_t::lock lock{grid_map};

	if (y < lock->elements.size())
	{
		const auto &row=lock->elements.at(y);

		if (x < row.size())
			return row.at(x)->grid_element;
	}
	return elementptr();
}

void gridlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	// Not all recalculation is the result of inserting or removing
	// elements. rebuild_elements() will do its work only if needed.
	bool flag=rebuild_elements(IN_THREAD);

	if (flag)
		initialize_new_elements(IN_THREAD);

	// recalculate_metrics would want to update the container's own
	// metrics, as the result of the recalculation.

	auto my_metrics=metrics::horizvert(container_impl->get_element_impl()
					   .get_horizvert(IN_THREAD));

#ifdef CALLING_RECALCULATE
	CALLING_RECALCULATE();
#endif

	if (!grid_elements(IN_THREAD)->recalculate_metrics(IN_THREAD,
							   flag, my_metrics))
		return;

	// Even though the current position hasn't changed, we need to
	// recalculate and reposition our display elements.
	current_position_updated(IN_THREAD);
#ifdef GRIDLAYOUTMANAGER_RECALCULATE_LOG
	GRIDLAYOUTMANAGER_RECALCULATE_LOG(grid_elements(IN_THREAD));
#endif
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

	if (info.row >= lock->elements.size())
		throw EXCEPTION(_("Attempting to add a display element into a nonexistent row"));

	auto &row=lock->elements.at(dim_t::value_type(info.row));

	if (info.col > row.size())
		throw EXCEPTION(_("Attempting to add a display element into a nonexistent column"));
	auto elem=grid_element::create(info, new_element,
				       grid_element_padding_lock{
					       container_impl
						       ->get_element_impl()
						       .get_screen()});

	if (info.col >= row.size())
		row.push_back(elem);
	else
		row.insert(row.begin()+dim_t::value_type(info.col), elem);

	// Reset the next element to defaults.
	info=new_grid_element_info{info.row, info.col+1,
				   get_custom_border(border_infomm{})};
	lock->elements_have_been_modified();
}

current_border_impl gridlayoutmanagerObj::implObj
::get_custom_border(const border_infomm &info)
{
	return container_impl->get_window_handler().screenref
		->impl->get_custom_border(info);
}

current_border_impl gridlayoutmanagerObj::implObj
::get_theme_border(const std::experimental::string_view &id)
{
	return container_impl->get_window_handler().screenref
		->impl->get_theme_border(id);
}

void gridlayoutmanagerObj::implObj
::child_background_color_changed(IN_THREAD_ONLY,
				 const elementimpl &child)
{
	redraw_child_borders_and_padding(IN_THREAD, child);
}

void gridlayoutmanagerObj::implObj
::child_visibility_changed(IN_THREAD_ONLY,
			   struct inherited_visibility_info &info,
			   const elementimpl &child)
{
	// No need to redraw when the visibility change is the result of
	// mapping the top level window. The forthcoming exposure events
	// will take care of this.
	if (!info.do_not_redraw)
		redraw_child_borders_and_padding(IN_THREAD, child);
}

LIBCXXW_NAMESPACE_END
