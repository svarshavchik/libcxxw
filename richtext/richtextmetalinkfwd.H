/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextmetalinkfwd_H
#define richtextmetalinkfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class richtextmetalinkObj;
class richtextmetalinkBase;

//! A link, of some kind, associated with a portion of \ref richtextmeta "rich text metadata".

//! The \ref richtextmetalinkObj "object" defines an interface for
//! a subclass to implement highlighting of a portion of a label, and
//! a callback to invoke when some action, such as clicking on it, happens.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::w::richtextmeta markup;
//!
//! auto link=INSERT_LIBX_NAMESPACE::w::richtextmetalink::create([],
//!                                          (INSERT_LIBX_NAMESPACE::w::richtextmeta &markup)
//!                                          {
//!                                          },
//!                                          []
//!                                          (INSERT_LIBX_NAMESPACE::w::richtextmetalink::base::event_t)
//!                                          {
//!                                          });
//! \endcode
//!
//! The default create() method constructs a subclass that
//! implements the override_text_markup() and
//! event() callbacks using the passed in functors.

typedef ref<richtextmetalinkObj, richtextmetalinkBase> richtextmetalink;

//! A nullable pointer reference to a \ref richtextmetalink "metadata link".

typedef ptr<richtextmetalinkObj, richtextmetalinkBase> richtextmetalinkptr;

LIBCXXW_NAMESPACE_END

#endif
