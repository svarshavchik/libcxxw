/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanagerobj_h
#define x_w_gridlayoutmanagerobj_h

#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/elementobj.H>
#include <x/w/types.H>
#include <x/w/border_infomm.H>
#include <x/mpobj.H>
#include <x/refptr_hash.H>

#include <vector>
#include <unordered_map>
#include <experimental/string_view>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see gridlayoutmanager

class gridlayoutmanagerObj : public layoutmanagerObj {

public:

	//! Mutex-protected internal grid data.

	typedef mpobj<ref<grid_map_infoObj>, std::recursive_mutex> grid_map_t;

private:
	//! An internal lock.

	grid_map_t::lock lock;

public:
	friend class gridfactoryObj;

	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	gridlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~gridlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append a row to the grid, and create elements in it.
	gridfactory append_row();

	//! Append elements at the end of an existing row.
	gridfactory append_columns(size_t row_number);

	//! Remove everything from the grid.
	void erase();

	//! Remove a display element from the given location.
	void erase(size_t x, size_t y);

	//! Get the display element at the given location.
	elementptr get(size_t x, size_t y) const;

	//! Set the default border above this row.
	void default_row_border(size_t row, const border_infomm &info);

	//! Set the default border above this row.
	void default_row_border(size_t row,
				const std::experimental::string_view &name);

	//! Set the requested row height.

	//! The row height is specified as a percentage, 0-100, of the total
	//! height of the grid.
	void requested_row_height(size_t row, int percentage);

	//! Set the default border to the left of this column
	void default_col_border(size_t col, const border_infomm &info);

	//! Set the default border to the left of this column
	void default_col_border(size_t col,
				const std::experimental::string_view &name);

	//! Set the requested column width.

	//! The column width is specified as a percentage, 0-100, of the total
	//! width of the grid.
	void requested_col_width(size_t col, int percentage);

	//! Remove all custom row defaults.
	void remove_row_defaults(size_t row);

	//! Remove all custom column defaults.
	void remove_col_defaults(size_t col);
};

LIBCXXW_NAMESPACE_END

#endif
