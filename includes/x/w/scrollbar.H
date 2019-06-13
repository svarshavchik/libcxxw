/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_scrollbar_h
#define x_w_scrollbar_h

#include <x/w/scrollbarfwd.H>
#include <x/w/scrollbarobj.H>
#include <x/w/containerobj.H>
#include <x/w/dim_arg.H>
#include <x/w/callback_triggerfwd.H>
#include <x/w/busyfwd.H>
#include <x/functionalrefptr.H>

#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Information passed to scroll-bar callbacks.

//! When dragging the scroll-bar by its handle, the official value
//! of the scroll-bar changes only when the scroll-bar handle gets
//! released. \c dragged_value reflects the value of the scroll-bar
//! as its being dragged, while \c value remains unchanged.
//!
//! The callback has two options for processing scroll-bar callback events:
//!
//! - handle only events when value and dragged_value are the same. This
//! results in the callback not responding while the scroll-bar is
//! dragging, until the scroll-bar's handle gets released.
//!
//! - handle all events, and always used dragged_value. This results in
//! the callback responding to the scroll-bar while it's dragged, and to all
//! other events that update the scroll-bar position.

struct LIBCXX_HIDDEN scrollbar_info_t {

	//! The current value, ignoring any dragging going on.
	scroll_v_t::value_type value;

	//! The current value being dragged. Same as value when not dragging.

	scroll_v_t::value_type dragged_value;

	//! Why the trigger got invoked.
	const callback_trigger_t &trigger;

	//! Busy mcguffin.
	const busy &mcguffin;
};

//! Scroll-Bar's initial configuration.

//! This is specified when creating a new scroll-bar, or when reconfigure()ing it.

struct scrollbar_config {

	//! The range of the scroll-bar's value.

	scroll_v_t range=1;

	//! Scroll-Bar page size.

	//! This translates to the size of the scroll-bar's handle. Must be
	//! at least 1.
	scroll_v_t page_size=1;

	//! Increment size.

	//! How much the scroll-bar's up and down buttons adjust its value
	scroll_v_t increment=1;

	//! The current value of the scroll-bar.

	//! When the scroll-bar gets created, this is the initial value.
	//! The current value of the scroll-bar will always be
	//! between 0 and range-page_size.
	scroll_v_t value=0;

	//! Minimum scrollbar size
	dim_arg minimum_size=0;

	//! Comparison operator.
	bool operator==(const scrollbar_config &o) const
	{
		return range == o.range &&
			page_size == o.page_size &&
			value == o.value &&
			increment == o.increment &&
			minimum_size == o.minimum_size;
	}
};

//! Specify when a scroll-bar is visible.

enum class scrollbar_visibility {
	//! Never show the scroll-bar.
	never,
	//! Always show the scroll-bar, even when there's nothing to scroll.
	always,
	//! The scroll-bar appears only when there's something to scroll.
	automatic,
	//! Same as \c automatic, but reserve the space for the scroll-bar in advance.

	//! The scroll-bar is not visible when there's nothing to scroll,
	//! and a blank area appears in its place.
	automatic_reserved
};

LIBCXXW_NAMESPACE_END

#endif
