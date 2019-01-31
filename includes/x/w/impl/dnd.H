/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_dnd_h
#define x_w_impl_dnd_h

#include <xcb/xproto.h>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! Supported dnd formats advertised by the source.

//! The formats given in the XdndEnter message, and from the source window's
//! XdndTypeList are combined together.

typedef std::unordered_set<xcb_atom_t> source_dnd_formats_t;

LIBCXXW_NAMESPACE_END

#endif
