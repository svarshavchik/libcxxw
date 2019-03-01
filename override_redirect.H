/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_override_redirect_h
#define x_w_override_redirect_h

#include "override_redirectfwd.H"
#include "override_redirectobj.H"
#include "generic_window_handler.H"
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

/*! Internally-controlled top level display element with Override-Redirect

The override_redirect_elementObj template multiply-inherits from
override_redirectObj and a generic_windowObj::handlerObj representing a
top level display element that has override redirect set.

The template mixin implements additional handling of override-redirected
window popups.

The template parameter must be a generic_windowObj::handlerObj or a subclass
of it.

The subclass must implement recalculate_window_position() to compute the
location of the window on the screen. recalculate_window_position() reiceves
a rectangle whose width and height is set to the preferred size of the
override-redirected window and the size of the screen.

recalculate_window_position() must set the rectangle's x and y coordinates,
and may also adjust the size, too.

*/

template<typename element_impl>
class override_redirect_elementObj : public element_impl,
				     public override_redirectObj {

public:

	//! Inherited constructor

	//! The constructor must be invoked with the override redirect flag
	//! set.

	using element_impl::element_impl;

	//! Destructor

	~override_redirect_elementObj()=default;

	//! Implement get_override_redirected_window_handler().

	generic_windowObj::handlerObj &get_override_redirected_window_handler()
		override
	{
		return *this;
	}

	//! Override frame_extends_updated()

	//! We might need to set_override_redirected_window_position().

	void frame_extents_updated(ONLY IN_THREAD) override
	{
		element_impl::frame_extents_updated(IN_THREAD);
		this->set_override_redirected_window_position(IN_THREAD);
	}

	//! Override horizvert_updated()

	//! We might need to set_override_redirected_window_position().

	void horizvert_updated(ONLY IN_THREAD) override
	{
		element_impl::horizvert_updated(IN_THREAD);
		this->set_override_redirected_window_position(IN_THREAD);
	}

	//! Override

	//! We might need to set_override_redirected_window_position().
	void theme_updated(ONLY IN_THREAD,
			   const defaulttheme &new_theme) override
	{
		element_impl::theme_updated(IN_THREAD, new_theme);

		this->set_override_redirected_window_position(IN_THREAD);
	}

	//! Override request_visibility()

	//! We actually set_override_redirected_window_position()
	//! only when we're visible.
	//!
	//! This optimizes recalculations that might occur several times
	//! when the popup is being created.
	//!
	//! This means that when requested visibility changes, we need
	//! to set_override_redirected_window_position(), for real.

	void request_visibility(ONLY IN_THREAD, bool flag) override
	{
		element_impl::request_visibility(IN_THREAD, flag);
		this->set_override_redirected_window_position(IN_THREAD);
	}

	//! Override process_configure_notify()

	//! We set_override_redirected-window_position() and manage
	//! our own size, so we will ignore ConfigureNotify messages
	//! from the server.

	void process_configure_notify(ONLY IN_THREAD) override
	{
		// Ignoring the ConfigureNotify event, see?
	}

	//! Override configure_notify_received()

	//! We set_override_redirected-window_position() and manage
	//! our own size, so we will ignore ConfigureNotify messages
	//! from the server.

	void configure_notify_received(ONLY IN_THREAD, const rectangle &)
		override
	{
		// Ignoring the ConfigureNotify event, see?
	}

	//! Override update_resizing_timeout().

	//! set_override_redirected-window_position() may end up
	//! adjusting our position outside of the requested metrics.

	void update_resizing_timeout(ONLY IN_THREAD) override
	{
		this->resizing(IN_THREAD)=
			!this->data(IN_THREAD).requested_visibility;
	}
};

LIBCXXW_NAMESPACE_END

#endif
