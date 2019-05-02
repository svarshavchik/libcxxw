/*
** Copyright 2014-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_gcfwd_H
#define x_w_gcfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC gcObj;
class LIBCXX_PUBLIC gcBase;

/*! A graphic context

Implements primitive graphic context operations, as described in the X protocol
extension. This library uses this
low-level object mostly to compose alpha masks in a pixmap.

\code
INSERT_LIBX_NAMESPACE::w::gc context;

INSERT_LIBX_NAMESPACE::w::gc::base::properties props;

context->fill_rectangle(0, 0, 100, 100, props);

context->segment(0, 0, 100, 100, props);

std::vector<INSERT_LIBX_NAMESPACE::w::polyline> lines;

context->lines(lines, props);

INSERT_LIBX_NAMESPACE::w::gc::arc one_arc;
std::vector<INSERT_LIBX_NAMESPACE::w::gc::arc> arcs;

context->draw_arc(one_arc, props);

context->draw_arcs(arcs, props);

context->fill_arc(one_arc, props);

context->fill_arcs(arcs, props);

context->copy(rectangle{0, 0, 50, 50}, 10, 10);

\endcode

Various overloaded versions of these methods implement optional features
of each X protocol drawing request. These methods require a
\ref gcObj::properties "INSERT_LIBX_NAMESPACE::w::gc::base::properties" object
that specify the particular graphic context settings which apply for that
operation.

*/

typedef ref<gcObj, gcBase> gc;

//! A nullable pointer reference to an \ref gc "graphic context"

typedef ptr<gcObj, gcBase> gcptr;

//! A constant \ref gc "graphic context"
typedef const_ref<gcObj, gcBase> const_gc;

//! A nullable pointer reference to a constant \ref gc "graphic context"

typedef const_ptr<gcObj, gcBase> const_gcptr;

LIBCXXW_NAMESPACE_END

#endif
