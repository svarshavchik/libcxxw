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
#include <x/w/alignment.H>
#include <x/refptr_hash.H>

#include <vector>
#include <unordered_map>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see gridlayoutmanager

class gridlayoutmanagerObj : public layoutmanagerObj {

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

	//! Insert a row in the grid before another row, then create elements in it.

	gridfactory insert_row(size_t row_number);

	//! Remove all columns from an existing row, then create new elements in the row.

	gridfactory replace_row(size_t row_number);

	//! Append elements at the end of an existing row.
	gridfactory append_columns(size_t row_number);

	//! Insert elements before an existing element on an existing row.
	gridfactory insert_columns(size_t row_number, size_t col_number);

	//! Remove everything from the grid.
	void remove();

	//! Remove a display element from the given location.

	//! The given column does not count spanned cells.
	void remove(size_t row, size_t col);

	//! Remove an entire row

	//! Does not remove any cells from prior rows that span the row.
	//!
	//! Does not adjust the row span value of cells that span vertically
	//! into this row, those cells still span the given number of rows,
	//! in the adjusted grid.

	void remove_row(size_t row);

	//! How many rows there are here.
	size_t rows();

	//! How many defined cells there are in the given row.

	//! This does not include spanned cells.
	size_t cols(size_t row);

	//! Get the display element at the given location.

	//! The column does not count spanned cells.
	elementptr get(size_t row, size_t col) const;

	//! Set the default border above this row.
	void default_row_border(size_t row, const border_infomm &info);

	//! Set the default border above this row.
	void default_row_border(size_t row,
				const std::string_view &name);

	//! Set the requested row height.

	//! The row height is specified as a percentage, 0-100, of the total
	//! height of the grid.
	void requested_row_height(size_t row, int percentage);

	//! Set the default vertical alignment of elements in the row.

	//! This must be set before creating elements in this row.
	void row_alignment(size_t row, valign alignment);

	//! Set the default border to the left of this column
	void default_col_border(size_t col, const border_infomm &info);

	//! Set the default border to the left of this column
	void default_col_border(size_t col,
				const std::string_view &name);

	//! Set the requested column width.

	//! The column width is specified as a percentage, 0-100, of the total
	//! width of the grid.
	void requested_col_width(size_t col, int percentage);

	//! Set the default horizontal alignment of elements in the column.

	//! This must be set before creating elements in this column.
	void col_alignment(size_t row, halign alignment);

	//! Remove all custom row defaults.
	void remove_row_defaults(size_t row);

	//! Remove all custom column defaults.
	void remove_col_defaults(size_t col);
};

LIBCXXW_NAMESPACE_END

#endif
