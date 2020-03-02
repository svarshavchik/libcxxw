/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_in_thread_new_cells_infofwd_h
#define x_w_in_thread_new_cells_infofwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN in_thread_new_cells_infoObj;

/*! Object with a new_cells_info.

listlayoutmanager's non-IN_THREAD methods that modify the contents of the
list construct this object with a new_cells_info, which gets forwarded to
the connection thread, which proceeds and implements the IN_THREAD version
of the method.

The IN_THREAD versions construct a new_cells_info directly on the stack.

*/

typedef ref<in_thread_new_cells_infoObj> in_thread_new_cells_info;

//! A constant \ref in_thread_new_cells_info "new_cells_info object".

//! \see in_thread_new_cells_info

typedef const_ref<in_thread_new_cells_infoObj> const_in_thread_new_cells_info;

//! A nullable pointer reference to a \ref in_thread_new_cells_info "new_cells_info object".

//! \see in_thread_new_cells_info

typedef ptr<in_thread_new_cells_infoObj> in_thread_new_cells_infoptr;

//! A nullable pointer reference to a const \ref in_thread_new_cells_info "new_cells_info object".

//! \see in_thread_new_cells_info

typedef const_ptr<in_thread_new_cells_infoObj> const_in_thread_new_cells_infoptr;

LIBCXXW_NAMESPACE_END

#endif
