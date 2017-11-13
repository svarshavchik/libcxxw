/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef scrollbar_h
#define scrollbar_h

#include "scrollbar/scrollbarfwd.H"
#include "scrollbar/scrollbarobj.H"
#include "scrollbar/scrollbar_metrics.H"
#include <x/ref.H>
#include <x/functional.H>

#include "x/w/containerobj.H"
#include "x/w/callback_triggerfwd.H"
#include "x/w/busyfwd.H"
#include <functional>

LIBCXXW_NAMESPACE_START

//! Information passed to scrollbar callbacks.

//! When dragging the scrollbar by its handle, the official value
//! of the scrollbar changes only when the scrollbar handle gets
//! released. \c dragged_value reflects the value of the scrollbar
//! as its being dragged, while \c value remains unchanged.
//!
//! The callback has two options for processing scrollbar callback events:
//!
//! - handle only events when value and dragged_value are the same. This
//! results in the callback not responding while the scrollbar is
//! dragging, until the scrollbar's handle gets released.
//!
//! - handle all events, and always used dragged_value. This results in
//! the callback responding to the scrollbar while it's dragged, and to all
//! other events that update the scrollbar position.

struct LIBCXX_HIDDEN scrollbar_info_t {

	scroll_v_t::value_type value;

	scroll_v_t::value_type dragged_value;

	const callback_trigger_t &trigger;

	const busy &mcguffin;
};

//! Callback that reports the scrollbar's updated values.
typedef std::function<void (const scrollbar_info_t &)> scrollbar_cb_t;

//! Scrollbar's initial configuration.

//! This is specified when creating a new scrollbar, or when updating it.

struct scrollbar_config {

	//! The range of the scrollbar's value.

	scroll_v_t range=1;

	//! Scrollbar page size.

	//! This translates to the size of the scrollbar's handle. Must be
	//! at least 1.
	scroll_v_t page_size=1;

	//! The current value of the scrollbar.

	//! When the scrollbar gets created, this is the initial value.
	//! The current value of the scrollbar will always be
	//! between 0 and range-page_size.
	scroll_v_t value=0;

	//! Increment size.

	//! How much the scrollbar's up and down buttons adjust its value
	scroll_v_t increment=1;

	//! Comparison operator.
	bool operator==(const scrollbar_config &o) const
	{
		return range == o.range &&
			page_size == o.page_size &&
			value == o.value &&
			increment == o.increment;
	}
};

//! Create a horizontal scrollbar.

//! To do that, we need the scrollbar's parent container,
//! a \ref scrollbar_config "scrollbar_config", and the scrollbar's
//! initial callback.
//!
//! This only creates the scrollbar element linked to the parent_container.
//! It is the caller's responsibility to, at some point, add it to the
//! parent using created_internally().

scrollbar do_create_h_scrollbar(const ref<containerObj::implObj>
				&parent_container,
				const scrollbar_config &conf,
				const scrollbar_cb_t &callback)
	LIBCXX_HIDDEN;

//! Create a vertical scrollbar.

//! Like create_horizontal_scrollbar(), but makes a vertical one.

scrollbar do_create_v_scrollbar(const ref<containerObj::implObj>
				&parent_container,
				const scrollbar_config &conf,
				const scrollbar_cb_t &callback)
	LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
