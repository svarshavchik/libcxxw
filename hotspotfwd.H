/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspotfwd_h
#define hotspotfwd_h

#include "x/w/hotspotobj.H"
#include <unordered_map>

LIBCXXW_NAMESPACE_START

//! A container for hotspots with hotkeys.

//! This map is kept in the top level generic window object handler.
//!
//! A hotspot with a shortcut installs itself here. Like
//! the focusable_fields list, the public object is responsible for
//! taking care of the circular reference.

typedef std::unordered_multimap<char32_t,
				ref<hotspotObj::implObj>
				> shortcut_lookup_t;


LIBCXXW_NAMESPACE_END

#endif
