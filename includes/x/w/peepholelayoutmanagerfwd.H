/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_peepholelayoutmanagerfwd_h
#define x_w_peepholelayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC peepholelayoutmanagerObj;

/*! The peephole layout manager

This layout manager puts another display element inside a (usually)
smaller area, and scrolls parts of it into view, as needed.

\see new_scrollable_peephole

\see new_peephole

*/

typedef ref<peepholelayoutmanagerObj> peepholelayoutmanager;

//! A constant \ref peepholelayoutmanager "peephole layout manager".

//! \see peepholelayoutmanager

typedef const_ref<peepholelayoutmanagerObj> const_peepholelayoutmanager;

//! A nullable pointer reference to a \ref peepholelayoutmanager "peephole layout manager".

//! \see peepholelayoutmanager

typedef ptr<peepholelayoutmanagerObj> peepholelayoutmanagerptr;

//! A nullable pointer reference to a const \ref peepholelayoutmanager "peephole layout manager".

//! \see peepholelayoutmanager

typedef const_ptr<peepholelayoutmanagerObj> const_peepholelayoutmanagerptr;

LIBCXXW_NAMESPACE_END

#endif
