/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_table_appearanceobj_h
#define x_w_table_appearanceobj_h

#include <x/w/table_appearancefwd.H>
#include <x/w/dim_arg.H>
#include <x/w/rgb.H>
#include <x/functional.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Actual properties inherited by \ref table_appearance "INSERT_LIBX_NAMESPACE::w::table_appearance".

struct table_appearance_properties {

	//! Constructor
	table_appearance_properties();

	//! Destructor
	~table_appearance_properties();

	//! Background color for the header row.
	color_arg header_color;

	//! Background color for the header row being adjusted.

	//! Header row's temporary background color when a border is being
	//! dragged, when adjustable_column_widths is set.
	color_arg adjustable_header_color;

	//! Highlighted backround color for the border being adjusted.

	color_arg adjustable_header_highlight_color;

	//! Width of the adjustable_header_highlight_color.

	//! This must agree with whatever went into
	//! adjustable_header_highlight_color. The highlight_color gets
	//! composited on top of the header_color in a one-pixel tall
	//! pixmap that spans the width of the header row. The highlight_color
	//! gets positioned behind the border being dragged. In order for
	//! the calculations to work out, this must correspond to the
	//! highlight_color (i.e. if the highlight_color is a 4mm wide
	//! gradient, this must be "4".

	dim_arg adjustable_header_highlight_width;

	//! Horizontal movement before column dragging starts

	//! Avoids quick errant clicks from dragging columns.
	dim_arg drag_horiz_start;

	//! Pointer image on top of an adjustable column divider.

	std::string adjust_cursor;
};

//! Implementation object.

//! \see table_appearance

class table_appearanceObj: public table_appearance_properties,
				 virtual public obj {

public:
	//! Constructor
	table_appearanceObj();

	//! Destructor
	~table_appearanceObj();

	//! Copy constructor
	table_appearanceObj(const table_appearanceObj &);

	//! Create a copy of this object.
	template<typename F> inline const_table_appearance modify(F &&f) const
	{
		return do_modify(make_function<void(const table_appearance &)>
				 (std::forward<F>(f)));
	}

	//! Type-erased modify().
	const_table_appearance do_modify(const function
					<void(const table_appearance &)> &)
		const;
};

LIBCXXW_NAMESPACE_END

#endif
