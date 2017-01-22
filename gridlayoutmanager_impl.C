/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridlayoutmanager_impl_elements.H"
#include "child_element.H"
#include "metrics_grid_pos.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::implObj
::implObj(const ref<containerObj::implObj> &container_impl)
	: layoutmanagerObj::implObj(container_impl),
	grid_elements_thread_only(ref<elementsObj>::create())
{
}

gridlayoutmanagerObj::implObj::~implObj()=default;

void gridlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	// Not all recalculation is the result of inserting or removing
	// elements. rebuild_elements() will do its work only if needed.
	bool flag=rebuild_elements(IN_THREAD);
}

void gridlayoutmanagerObj::implObj
::do_for_each_child(IN_THREAD_ONLY,
		    const function<void (const child_element &e)> &callback)
{
}

layoutmanager gridlayoutmanagerObj::implObj::create_public_object()
{
	return gridlayoutmanager::create(ref<implObj>(this));
}

void gridlayoutmanagerObj::implObj
::insert(const child_element &new_element,
	 dim_t x, dim_t y, dim_t width, dim_t height)
{
	if (width == 0 || height == 0)
		throw EXCEPTION(_("Width or height of a new grid element cannot be 0"));

	auto x1v=(dim_t::value_type)x, y1v=(dim_t::value_type)y;

	auto x2v=(dim_squared_t::value_type)(x+(width-1)),
		y2v=(dim_squared_t::value_type)(y+(height-1));

	auto grid_pos=metrics::grid_pos::create(metrics::grid_axisrange
						(x1v, x2v),
						metrics::grid_axisrange
						(y1v, y2v));

	grid_map_key_t key{x, y};

	grid_map_t::lock lock(grid_map);

	if (lock->elements.find(key) != lock->elements.end())
		throw EXCEPTION(_("Grid element already exists"));
	lock->elements.insert({key, {new_element, grid_pos}});
	lock->modified=true;
}

LIBCXXW_NAMESPACE_END
