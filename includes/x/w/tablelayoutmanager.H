/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanager_h
#define x_w_tablelayoutmanager_h

#include <x/w/tablelayoutmanagerfwd.H>
#include <x/w/tablelayoutmanagerobj.H>
#include <x/w/table_appearancefwd.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/labelfwd.H>
#include <x/w/text_paramfwd.H>
#include <cmath>
#include <string_view>
#include <string>
#include <vector>
#include <optional>

LIBCXXW_NAMESPACE_START

/*! Create a new table layout manager.

A subclass of the \ref listlayoutmanager "list layout manager" that
adds a header row to the list. Inherits from the
\ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager".

The new container uses the
\ref tablelayoutmanager "INSERT_LIBX_NAMESPACE::w::tablelayoutmanager".

The required constructor parameter is a vector of callable objects, one
for each column in the table. The constructor's calla ble object get used
to construct each column's header. This callback ends up getting
invoked by create_focusable_container() to populate each
column's header.

Each callback gets a \ref factory "factory" as its parameter.
The callback must use the given factory to construct exactly
one widget. The callback is responsible for show()ing
the widget.

Use \ref table_headers "INSERT_LIBX_NAMESPACE::w::table_headers" as a
shortcut for creating the header factories.

The size of the passed in vector initializes the \c columns value in the
inheritable \ref new_listlayoutmanager "new_listlayoutmanager".

This is not a thread-safe object.

*/

class new_tablelayoutmanager : public new_listlayoutmanager {

 public:

	typedef functionref<void (const factory &)> header_factory_t;

	//! Constructor

	//! Optional second parameter specifies the list style.

	new_tablelayoutmanager(
		std::vector<header_factory_t> header_factories,
		const listlayoutstyle_impl &list_style=highlighted_list)
		LIBCXX_PUBLIC;

	//! Destructor
	~new_tablelayoutmanager() LIBCXX_PUBLIC;

	//! Override set_pane_theme

	//! Installs INSERT_LIBX_NAMESPACE::w::list_appearance::base::table_pane_theme().

	void set_pane_theme() override LIBCXX_HIDDEN;

	//! Header factories

	//! This was passed to the constructor. This can be modified after
	//! construction and before calling create_focusable_container(), if
	//! \c columns in the inheritable
	//! \ref new_listlayoutmanager "new_listlayoutmanager" is also
	//! updated.

	std::vector<header_factory_t> header_factories;

	//! Header appearance.

	const_table_appearance extra_table_appearance;

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

	//! If set, a non-empty string gives a unique label for this table
	//! which is used to save the adjusted table column widths and
	//! restore them the next time. An empty string still enables
	//! adjustable column widths but doesn't save their widths, for
	//! next time.
	//!
	//! The bool flag enables restoring the previous column widths. If
	//! false the column widths get reset to default, this time, but
	//! their widths are saved and then restored the next time.

	std::optional<std::tuple<std::string, bool>> adjustable_column_widths;

	//! Enable saving and restoring the column widths.

	//! Shortcut for setting adjustable_column_widths.

	void adjustable(const std::string &name="",
			bool restore=true)
	{
		adjustable_column_widths.emplace(name, restore);
	}

	//! Used by the theme parser

	//! An optional \<norestore /> is used in theme files. The theme
	//! parser calls this helper.

	void norestore(const std::string &name, bool norestore)
	{
		adjustable_column_widths.emplace(name, !norestore);
	}

private:

	//! Create the focusable container.

	//! \internal
	focusable_container create(const container_impl &,
				   const function<void
				   (const focusable_container &)>
				   &creator)
		const override LIBCXX_HIDDEN;
private:

	//! Implement created_list_container().

	//! \internal
	void created_list_container(const container_impl &,
				    table_create_info *)
		const override LIBCXX_HIDDEN;

	//! Implement create_table_header_row()

	void create_table_header_row(const gridlayoutmanager &,
				     table_create_info *) const override
		LIBCXX_HIDDEN;
};

//! Parameter for table_headers()

typedef std::vector<std::tuple<x::w::text_param, x::w::label_config>
		    > table_headers_param_t;

/*! Create header factories for \ref new_tablelayoutmanager "new_tablelayoutmanager"'s constructor

\code
#include <INSERT_LIBX_NAMESPACE/w/label.H>

INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager new_table{
      INSERT_LIBX_NAMESPACE::w::table_headers(
          {
	      {"Column1", {}},
	      {"Column2", {}},
	  })
    };
\endcode

This is helper for creating a vector of factories for a table's columns.
table_headers() returns a vector for
\ref new_tablelayoutmanager "new_tablelayoutmanager"'s
constructor with factories that use \ref factory "create_label()" and show() it.

table_headers()'s parameter is a vector of tuples with two values:

- \ref text_param "INSERT_LIBX_NAMESPACE::w::text_param"

- \ref label_config "INSERT_LIBX_NAMESPACE::w::label_config"

*/

std::vector<new_tablelayoutmanager::header_factory_t> table_headers(
	const table_headers_param_t &table_headers_param
);

LIBCXXW_NAMESPACE_END

#endif
