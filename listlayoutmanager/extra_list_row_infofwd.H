/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_extra_list_row_infofwd_h
#define x_w_extra_list_row_infofwd_h

#include "x/w/namespace.H"
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN extra_list_row_infoObj;
struct LIBCXX_HIDDEN list_row_info_t;
struct LIBCXX_HIDDEN textlist_rowinfo;
struct LIBCXX_HIDDEN new_cells_info;

/*! Extra information about a particular row in the list.

\ref list_row_info_vector "A vector of list_row_info_t objects"
represents the list managed by the list layout manager.

This vector gets resized, as items are added to or removed from the list.
Each \ref list_row_info_t "list_row_info_t" has a pointer to this additional
object, and object remains where it is, while other list items are inserted or
deleted, and is not affected by changed to the contents of the list.

The extra_list_row_infoObj inherits from radio_buttonObj. For menus, each
list item that's a menu option in a radio group uses this to implement
radio button functionality.

*/

typedef ref<extra_list_row_infoObj> extra_list_row_info;

//! A constant \ref extra_list_row_info "extra list item information".

//! \see extra_list_row_info

typedef const_ref<extra_list_row_infoObj> const_extra_list_row_info;

//! A nullable pointer reference to a \ref extra_list_row_info "extra list item information".

//! \see extra_list_row_info

typedef ptr<extra_list_row_infoObj> extra_list_row_infoptr;

//! A nullable pointer reference to a const \ref extra_list_row_info "extra list item information".

//! \see extra_list_row_info

typedef const_ptr<extra_list_row_infoObj> const_extra_list_row_infoptr;

//! Items in a list can be enabled, disabled, or a separator.

enum class list_row_type_t { enabled, disabled, separator };

LIBCXXW_NAMESPACE_END

#endif
