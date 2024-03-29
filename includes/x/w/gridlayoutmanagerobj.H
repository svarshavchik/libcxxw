/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_gridlayoutmanagerobj_h
#define x_w_gridlayoutmanagerobj_h

#include <x/w/gridlayoutmanagerfwd.H>
#include <x/w/grid_map_infofwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/gridfactoryfwd.H>
#include <x/w/factoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/elementobj.H>
#include <x/w/types.H>
#include <x/w/border_arg.H>
#include <x/w/alignment.H>
#include <x/w/dim_arg.H>
#include <x/w/shortcutfwd.H>
#include <x/w/containerfwd.H>
#include <x/w/uielementsfwd.H>
#include <x/w/uigeneratorsfwd.H>
#include <x/w/rgbfwd.H>
#include <x/refptr_hash.H>
#include <x/functionalrefptrfwd.H>

#include <vector>
#include <unordered_map>
#include <string_view>
#include <string>

LIBCXXW_NAMESPACE_START

//! The grid layout manager

//! \see gridlayoutmanager

class gridlayoutmanagerObj : public layoutmanagerObj {

public:
	friend class gridfactoryObj;

	class LIBCXX_HIDDEN implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Lock on the underlying grid
	grid_map_t::lock grid_lock;

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

	//! Insert elements before an existing widget on an existing row.
	gridfactory insert_columns(size_t row_number, size_t col_number);

	//! Replace an existing widget
	gridfactory replace_cell(size_t row_number, size_t col_number);

	//! Remove everything from the grid.
	void remove();

	//! Remove a widget from the given location.

	//! The given column does not count spanned cells.
	void remove(size_t row, size_t col);

	//! Remove an entire row

	//! Does not remove any cells from prior rows that span the row.
	//!
	//! Does not adjust the row span value of cells that span vertically
	//! into this row, those cells still span the given number of rows,
	//! in the adjusted grid.

	void remove_row(size_t row);

	//! Remove rows

	//! Multiple rows version of remove_row().

	void remove_rows(size_t row, size_t n);

	//! How many rows there are here.
	size_t rows() const;

	//! How many defined cells there are in the given row.

	//! This does not include spanned cells.
	size_t cols(size_t row) const;

	//! Get the widget at the given location.

	//! The column does not count spanned cells.
	elementptr get(size_t row, size_t col) const;

	//! Find the row and column of the specified widget.

	//! Returns a tuple with the given element's row and column, if the
	//! widget exists in the grid.

	std::optional<std::tuple<size_t, size_t>
		      > lookup_row_col(const element &);

	//! Set the default border above this row.
	void default_row_border(size_t row, const border_arg &arg);

	//! Set the requested row height.

	//! The row height is specified as a percentage, 0-100, of the total
	//! height of the grid.
	void requested_row_height(size_t row, int percentage);

	//! Set the default vertical alignment of elements in the row.

	//! This must be set before creating elements in this row.
	void row_alignment(size_t row, valign alignment);

	//! Set the default border to the left of this column
	void default_col_border(size_t col, const border_arg &arg);

	//! Set the requested column width.

	//! The column width is specified as a percentage, 0-100, of the total
	//! width of the grid.
	void requested_col_width(size_t col, int percentage);

	//! Set the default horizontal alignment of elements in the column.

	//! This must be set before creating elements in this column.
	void col_alignment(size_t col, halign alignment);


	//! Set padding, specified in millimeters or as a theme dimension name.
	void row_top_padding(size_t row, const dim_arg &padding);

	//! Set padding, specified in millimeters or as a theme dimension name.
	void row_bottom_padding(size_t row, const dim_arg &padding);

	//! Set padding, specified in millimeters or as a theme dimension name.
	void col_left_padding(size_t col, const dim_arg &padding);

	//! Set padding, specified in millimeters or as a theme dimension name.
	void col_right_padding(size_t col, const dim_arg &padding);

	//! Remove all custom row defaults.
	void remove_row_defaults(size_t row);

	//! Remove all custom column defaults.
	void remove_col_defaults(size_t col);

	//! Set the container's background color
	void set_background_color(const color_arg &arg);

	//! Set the container's background color
	void remove_background_color();

	//! Resort the rows
	void resort_rows(const std::vector<size_t> &);

	//! Implement generate()

	//! \see uigenerators

	void generate(const std::string_view &name,
		      const const_uigenerators &generators,
		      uielements &elements) override;

	using layoutmanagerObj::generate;
};

LIBCXXW_NAMESPACE_END

#endif
