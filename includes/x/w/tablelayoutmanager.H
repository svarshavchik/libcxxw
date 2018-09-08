/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_tablelayoutmanager_h
#define x_w_tablelayoutmanager_h

#include <x/w/tablelayoutmanagerfwd.H>
#include <x/w/tablelayoutmanagerobj.H>
#include <x/w/listlayoutmanager.H>

LIBCXXW_NAMESPACE_START

//! The table layout manager.

//! \code
//! INSERT_LIBX_NAMESPACE::w::new_tablelayoutmanager ntlm{
//!     []
//!     (const INSERT_LIBX_NAMESPACE::w::factory &f,
//!      size_t column)
//!     {
//!           f->create_label("Header!")->show();
//!     }
//! };
//! \endcode
//!
//! A subclass of the \ref listlayoutmanager "list layout manager" that
//! adds a header row to the list. Inherits from the
//! \ref new_listlayoutmanager "INSERT_LIBX_NAMESPACE::w::new_listlayoutmanager".
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

class LIBCXX_HIDDEN new_tablelayoutmanager : public new_listlayoutmanager {

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

 private:

	//! Implement create_table_header_row()

	void create_table_header_row(const gridlayoutmanager &) const override;
};

LIBCXXW_NAMESPACE_END

#endif
