/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_override_redirectobj_h
#define x_w_override_redirectobj_h

#include "override_redirectfwd.H"
#include "x/w/rectangle.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see override_redirect

class override_redirectObj: virtual public obj {

public:
	//! Constructor
	override_redirectObj();

	//! Destructor
	~override_redirectObj();

	//! Return my mixed-in handler object.
	virtual generic_windowObj::handlerObj &
	get_override_redirected_window_handler()=0;

protected:

	//! Call recalculate_popup_position(), then xcb_configure_window().

	//! Assemble all the information that recalculate_popup_position()
	//! needs to base its logic on, call it, then use the adjusted
	//! rectangle to xcb_configure_window().

	void set_override_redirected_window_position(ONLY IN_THREAD);

public:
	//! Subclass implements recalculate_window_position().

	//! r is set to the default window position, which the subclass can
	//! inspect and make the necessary adjustments.
	//!
	//! The end result gets adjusted to make sure it fits on the screen,
	//! then sets the window position.

	virtual void recalculate_window_position(ONLY IN_THREAD,
						 rectangle &r,
						 dim_t screen_width,
						 dim_t screen_height)=0;

private:

	//! Our intended size.
	rectangle most_recent_configuration;
};

LIBCXXW_NAMESPACE_END

#endif
