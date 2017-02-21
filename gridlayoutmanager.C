/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "gridlayoutmanager.H"
#include "gridfactory.H"
#include "x/w/element.H"
#include "gridfactory.H"
#include "metrics_grid_pos.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

gridlayoutmanagerObj::gridlayoutmanagerObj(const ref<implObj> &impl)
	: layoutmanagerObj(impl), lock(impl->grid_map), impl(impl)
{
}

gridlayoutmanagerObj::~gridlayoutmanagerObj()=default;

const gridlayoutmanagerObj::grid_map_info_t::lookup_t &
gridlayoutmanagerObj::grid_map_info_t::get_lookup_table()
{
	if (!lookup_table_is_current)
	{
		// Clear the flags, first.

		for (const auto &info:lookup)
			info.second->seen=false;

		// Make a straightforward pass over the elements, counting
		// off each row and column, and update the lookup table.

		size_t row_number=0;

		for (const auto &row:elements)
		{
			size_t col_number=0;

			for (const auto &col:row)
			{
				auto iter=lookup.find(col->grid_element->impl);

				if (iter == lookup.end())
				{
					iter=lookup.insert
						({col->grid_element->impl,
						  lookup_info::create()})
						.first;
				}

				iter->second->row=row_number;
				iter->second->col=col_number;
				iter->second->seen=true;

				++col_number;
			}

			++row_number;
		}

		// Remove from the lookup table any elements that have been
		// removed.

		for (auto b=lookup.begin(), e=lookup.end(); b != e; )
		{
			if (b->second->seen)
			{
				++b;
				continue;
			}

			b=lookup.erase(b);
		}

		lookup_table_is_current=true;
	}

	return lookup;
}

gridfactory gridlayoutmanagerObj::create()
{
	auto me=gridlayoutmanager(this);

	return gridfactory::create(me,
				   ref<gridfactoryObj::implObj>::create(me));
}

void gridlayoutmanagerObj::erase(dim_t x, dim_t y)
{
}

elementptr gridlayoutmanagerObj::get(dim_t x, dim_t y)
{
	return elementptr();
}

LIBCXXW_NAMESPACE_END
