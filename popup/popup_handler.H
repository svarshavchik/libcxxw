/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef popup_handler_h
#define popup_handler_h

#include "popup/popup.H"
#include "generic_window_handler.H"
#include "connection_threadfwd.H"
#include "x/w/rectangle.H"

LIBCXXW_NAMESPACE_START

//! Popup handler implementation object.

//! Subclass generic_window's handler. Implement metrics methods to
//! invoke recalculate_popup_position(), then proceeds and sets the
//! top level display element's position accordingly.
//!
//! \see popup

class LIBCXX_HIDDEN popupObj::handlerObj :
	public generic_windowObj::handlerObj {

public:
	handlerObj(IN_THREAD_ONLY,
		const ref<generic_windowObj::handlerObj> &parent);
	~handlerObj();

	//! Override frame_extends_updated()

	//! This means we need to set_popup_position().

	void frame_extents_updated(IN_THREAD_ONLY) override;

	//! Override horizvert_updated()

	//! This means we need to set_popup_position().

	void horizvert_updated(IN_THREAD_ONLY) override;

	//! Implement theme_updated()
	void theme_updated(IN_THREAD_ONLY, const defaulttheme &new_theme)
		override;

	//! Recalculate what my position should be.

	//! r is set to the default popup position, which the subclass can
	//! inspect and make the necessary adjustments.
	//!
	//! The end result gets adjusted to make sure it fits on the screen,
	//! then sets the popup position.

	virtual void recalculate_popup_position(IN_THREAD_ONLY,
						rectangle &r,
						dim_t screen_width,
						dim_t screen_height)=0;

 private:

	//! Call recalculate_popup_position(), then xcb_configure_window().

	//! Assemble all the information that recalculate_popup_position()
	//! needs to base its logic on, call it, then use the adjusted
	//! rectangle to xcb_configure_window().

	void set_popup_position(IN_THREAD_ONLY);

 public:

	//! Override configure_notify()

	//! We manage our own size, and ignore ConfigureNotify messages
	//! from the server.
	void configure_notify(IN_THREAD_ONLY, const rectangle &r) override;

};

LIBCXXW_NAMESPACE_END

#endif