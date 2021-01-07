/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_hotspotfwd_H
#define x_w_text_hotspotfwd_H

#include <x/ptrfwd.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC text_hotspotObj;

//! A link, of some kind, associated with a portion of \ref label "label".

//! The \ref text_hotspotObj "object" defines an interface for
//! a subclass to implement highlighting of a portion of a label, and
//! a callback to invoke when some action, such as clicking on it, happens.
//!
//! \code
//! auto link=INSERT_LIBX_NAMESPACE::w::text_hotspot::create([],
//!                                          []
//!                                          (ONLY IN_THREAD,
//!                                           const INSERT_LIBX_NAMESPACE::w::text_event_t &e)
//!                                          {
//!                                              INSERT_LIBX_NAMESPACE::w::text_param ret;
//!
//!                                              return ret;
//!                                          });
//! \endcode

typedef ref<text_hotspotObj> text_hotspot;

//! A nullable pointer reference to a \ref text_hotspot "text link".

typedef ptr<text_hotspotObj> text_hotspotptr;

LIBCXXW_NAMESPACE_END

#endif
