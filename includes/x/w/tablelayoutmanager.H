/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanager_h
#define x_w_tablelayoutmanager_h

#include <x/w/tablelayoutmanagerfwd.H>
#include <x/w/tablelayoutmanagerobj.H>
#include <x/w/listlayoutmanager.H>
#include <cmath>

LIBCXXW_NAMESPACE_START

//! Create a new table layout manager.

//! A subclass of the \ref listlayoutmanager "list layout manager" that
//! adds a header row to the list. Inherits from the
//! \ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager".
//!
//! The new container uses the
//! \ref tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::tablelayoutmanager".
//!
//! The required constructor parameter is a callable object that gets used
//! to construct each column's header. This callback ends up getting
//! invoked by create_focusable_container() to populate each
//! column's header. The callback gets called once for each
//! column, with the \c column parameter varying from 0 to
//! columns-1.
//!
//! The callback must use the given factory to construct exactly
//! one display element. The callback is responsible for show()ing
//! the element.
//!
//! This is not a thread-safe object.

class new_tablelayoutmanager : public new_listlayoutmanager {

 public:

	//! Constructor

	//! Optional second parameter specifies the list style.

	new_tablelayoutmanager(const functionref<void (const factory &, size_t)
			       > &header_factory,
			       const listlayoutstyle_impl &list_style
			       =highlighted_list)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_tablelayoutmanager() LIBCXX_PUBLIC;

	//! The header factory that was passed to the constructor.

	functionref<void (const factory &, size_t)> header_factory;

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

	//! A non-default table size, in millimeters.

	//! A width of 0 means use the default initial table size.
	double table_width=0;

	//! Maximum table width.
	double maximum_table_width=0;

	//! Specify an unlimited table width.
	void unlimited_table_width()
	{
		maximum_table_width=NAN;
	}

	//! Whether the table borders can be dragged to adjust column widths.

	bool adjustable_column_widths=false;

	//! Restore previously save()d columns widths.

	void restore(const screen_positions &,
		     const std::string &) LIBCXX_PUBLIC;

	//! Restored widths.

	//! \internal
	//!
	//! Use restore() to populate this.
	std::vector<dim_t> restored_widths;

 private:

	//! Create the focusable container.

	//! \internal
	focusable_container create(const container_impl &)
		const override;
 private:

	//! Implement created_list_container().

	//! \internal
	virtual void created_list_container(const container_impl &,
					    table_create_info *)
		const override;
	//! Implement create_table_header_row()

	void create_table_header_row(const gridlayoutmanager &,
				     table_create_info *) const override;
};

LIBCXXW_NAMESPACE_END

#endif
