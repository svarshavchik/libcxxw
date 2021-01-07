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
#include <cmath>
#include <string_view>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN new_tablelayoutmanager_restored_position;

//! Restored table layout manager configuration

//! Implements \ref new_tablelayoutmanager "new_tablelayoutmanager"'s
//! restore() method.

struct new_tablelayoutmanager_restored_position {

	//! Restore previously save()d columns widths.

	//! The 2nd parameter is a unique identifier/label for this table.
	//! All tables should have unique identifiers. This label also gets
	//! used when save()ing window's position.
	void restore(const const_screen_positions &,
		     const std::string_view &) LIBCXX_PUBLIC;

	//! Table's name

	//! \internal
	//!
	//! The name that was passed to restore().
	std::string name;

	//! Restored widths.

	//! \internal
	//!
	//! Use restore() to populate this.
	std::vector<dim_t> restored_widths;
};

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
//! one widget. The callback is responsible for show()ing
//! the widget.
//!
//! This is not a thread-safe object.

class new_tablelayoutmanager : public new_listlayoutmanager,
			       public new_tablelayoutmanager_restored_position {

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

	//! Override set_pane_theme

	//! Installs INSERT_LIBX_NAMESPACE::w::list_appearance::base::table_pane_theme().

	void set_pane_theme() override LIBCXX_HIDDEN;

	//! The header factory that was passed to the constructor.

	functionref<void (const factory &, size_t)> header_factory;

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

	bool adjustable_column_widths=false;

	using new_tablelayoutmanager_restored_position::restore;

	//! Helper used by the theme parser.

	inline void restore(const new_tablelayoutmanager_restored_position &rp)
	{
		static_cast<new_tablelayoutmanager_restored_position &>(*this)
			=rp;
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

LIBCXXW_NAMESPACE_END

#endif
