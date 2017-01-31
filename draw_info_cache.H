/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef draw_cache_h
#define draw_cache_h

#include <unordered_map>

#include "element.H"
#include "draw_info.H"
#include <x/refptr_hash.H>

LIBCXXW_NAMESPACE_START

//! A cache of per-element drawi metadata.

//! Constructed on the connection thread's stack. This cache persists
//! for the duration of various callbacks. No callbacks except exposure_event()
//! and explicit_redraw() should be touching this.
//!
//! This caches per-element results of get_draw_info().
//!
//! redraw_elements(), which makes the primary use of this cache, gets invoked
//! as the last order of business in run_something(), and should be using this
//! cache after any changes to the display elements made by other callbacks.

struct draw_info_cache {

	std::unordered_map<ref<elementObj::implObj>, draw_info> draw_info_cache;

};

LIBCXXW_NAMESPACE_END

#endif
