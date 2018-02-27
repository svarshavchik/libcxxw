/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef background_color_element_recalculatedfwd_H
#define background_color_element_recalculatedfwd_H

#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! Define the background_color_element_recalculated() callback.

//! The template parameter is a background_color_elementObj template.
//! The templated class derives from the template parameter.
//!
//! Defines a background_color_element_recalculated() callback that gets
//! invoked whenever, basically, the get_background_color_for() gets
//! invoked in all of the inherited background_colors. If the superclass
//! multiply-inherits from a bunch of background_colors, an event ends
//! up invoking each one's, but background_color_element_recalculated()
//! gets invoked only once.
//!
//! \note
//! background_color_element_recalculated() does not get called as a result
//! of invoking one background color's update(). This gets invoked only if
//! all background_colors were recalculated as a result of an event.

template<typename element_impl>
class background_color_element_recalculatedObj : public element_impl {

	//! Flag that gets set by some_background_color_recalculated().

	bool recalculated_flag;

public:

	using element_impl::element_impl;

	~background_color_element_recalculatedObj();

	void initialize(IN_THREAD_ONLY) override;

	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
		override;

	void process_updated_position(IN_THREAD_ONLY) override;

	void invoke_element_state_updates(IN_THREAD_ONLY,
					  element_state::state_update_t)
		override;

	void background_color_was_recalculated(IN_THREAD_ONLY)
		override;

	//! Callback: all background colors have been recalculated.
	virtual void all_background_colors_were_recalculated(IN_THREAD_ONLY)=0;
};

LIBCXXW_NAMESPACE_END

#endif
