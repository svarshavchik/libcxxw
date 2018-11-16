/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "grid_map_info.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/element.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

grid_map_column_defaults::grid_map_column_defaults()=default;

grid_map_column_defaults::~grid_map_column_defaults()=default;

grid_map_column_defaults
::grid_map_column_defaults(const grid_map_column_defaults &)=default;

grid_map_column_defaults::grid_map_column_defaults(grid_map_column_defaults &&)
=default;

grid_map_column_defaults &grid_map_column_defaults
::operator=(const grid_map_column_defaults &)=default;

grid_map_column_defaults &grid_map_column_defaults
::operator=(grid_map_column_defaults &&)=default;

grid_map_row_defaults::grid_map_row_defaults()=default;

grid_map_row_defaults::~grid_map_row_defaults()=default;

grid_map_row_defaults
::grid_map_row_defaults(const grid_map_row_defaults &)=default;

grid_map_row_defaults::grid_map_row_defaults(grid_map_row_defaults &&)
=default;

grid_map_row_defaults &grid_map_row_defaults
::operator=(const grid_map_row_defaults &)=default;

grid_map_row_defaults &grid_map_row_defaults
::operator=(grid_map_row_defaults &&)=default;

grid_map_infoObj::grid_map_infoObj()=default;

grid_map_infoObj::~grid_map_infoObj()=default;

const grid_map_infoObj::lookup_t &grid_map_infoObj::get_lookup_table()
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

size_t grid_map_infoObj::cols(size_t row) const
{
	if (row >= elements.size())
		throw EXCEPTION(_("Attempting to get the number of columns in a nonexistent row"));

	return elements[row].size();
}

elementptr grid_map_infoObj::get(size_t row, size_t col) const
{
	if (row < elements.size())
	{
		const auto &r=elements[row];

		if (col < r.size())
			return r[col]->grid_element;
	}
	return elementptr{};
}

LIBCXXW_NAMESPACE_END
