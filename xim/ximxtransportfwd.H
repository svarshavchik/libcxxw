/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef ximxtransportfwd_H
#define ximxtransportfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN ximxtransportObj;

//! An interface to the X Input Method server.

//! Owns two objects: an implementation object that's an INPUT_ONLY window,
//! and an windowObj, that owns the INPUT_ONLY window.
//!
//! A reference to this object is stored in the generic_window's implementation
//! object, which owns it.
//!
//! \c generic_windowObj::implObj's constructor creates this object. A weak
//! pointer to this transport object is stored in the connection object, and
//! gets constructed on demand. As such, this is a per-connection singleton
//! object, that's shared by all generic_windows.
//!
//! Each generic_window, in addition to owning a ref to the singleton object,
//! also owns its own, per-generic_window, \ref ximclient "ximclient" object
//! that represents that generic_window as an XIM client.
//!
//! After this object's constructor creates its implementation object and
//! a windowObj, the implementation object's connect() gets invoked, which
//! starts the process of connecting and negotiating pleasantries with the
//! system's XIM server.
//!
//! This object's destructor also sets the gears in motion to untangle itself
//! from the XIM server.
//!
//! Since this is a per-connection singleton that's owned by generic_windows
//! when this object's destructor runs it means that all windows have been
//! destroyed, or, rather, are in the process of being destroyed. This is
//! important, since for everything to be fully destroyed it is necessary
//! to untangle some circular references and dependencies between the
//! implementation object, and generic_window handler objects (both
//! talk to each other), and the orderly shutdown is responsible for making
//! this happen.

typedef ref<ximxtransportObj> ximxtransport;

//! A nullable pointer reference to a \ref ximxtransport "XIM xtransport".

typedef ptr<ximxtransportObj> ximxtransportptr;

LIBCXXW_NAMESPACE_END
#endif
