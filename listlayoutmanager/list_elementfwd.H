/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_list_elementfwd_h
#define x_w_list_elementfwd_h

#include "x/w/namespace.H"
#include "x/w/types.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN list_elementObj;

/*! The singleton element managed by the \ref listlayoutmanager "list layout manager".

The list layout manager uses the layout manager framework to manage list items.

List items are not discrete display elements; but within the framework
the list layout manager is set up as containing this display element.

The \ref list_element "list element" internally manages individual cell
objects, that represent the items in the list, with the list layout manager
using them to implement the list concept.

*/

typedef ref<list_elementObj> list_element;

//! A constant \ref list_element "text list element".

//! \see list_element

typedef const_ref<list_elementObj> const_list_element;

//! A nullable pointer reference to a \ref list_element "text list element".

//! \see list_element

typedef ptr<list_elementObj> list_elementptr;

//! A nullable pointer reference to a const \ref list_element "text list element".

//! \see list_element

typedef const_ptr<list_elementObj> const_list_elementptr;

//! How tall is the tallest row managed by the list layout manager.

//! Sometimes we want to know the height of the tallest row with padding,
//! sometimes without including the padding between the rows.
//!
//! Each row in the list layout manager has padding above and below it.
//! When the list is in the peephole, we want the peephole's scroll buttons
//! to scroll the peephole by one row, this should include the padding, so
//! the peephole scrolls by the height of the row itself, plus the padding to
//! get to the next row.
//!
//! The combo-box layout manager also sized the current list element to the
//! height of the tallest row, but that should not include any padding, so that
//! it uses just the height of the row.

struct LIBCXX_HIDDEN tallest_row_height_t {

        //! The height of the tallest row, full stop.
	dim_t without_padding;

	//! The height of the tallest row, plus padding between the rows.
	dim_t with_padding;

	//! Comparison operator.
	inline bool operator==(const tallest_row_height_t &o) const
	{
		return without_padding == o.without_padding &&
			with_padding == o.with_padding;
	}
};

LIBCXXW_NAMESPACE_END

#endif
