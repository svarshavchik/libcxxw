/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_h
#define hotspot_h

#include "x/w/hotspotobj.H"
#include "x/w/elementobj.H"
#include "x/w/input_mask.H"
#include "x/w/focusablefwd.H"
#include "x/w/key_eventfwd.H"
#include "x/w/button_eventfwd.H"
#include "x/w/shortcut.H"
#include "x/w/focus.H"
#include "x/w/generic_windowobj.H"
#include "x/w/impl/focus/focusable_elementfwd.H"
#include "x/w/connection_threadfwd.H"
#include "activated_in_threadobj.H"
#include <x/refptr_traits.H>
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

class child_elementObj;

//! Hotspot implementation.

//! The hotspot_elementObj template inherits from this class and a subclass
//! of child_elementObj, and implements get_hotspot_element().

class LIBCXX_HIDDEN hotspotObj::implObj
	: public activated_in_threadObj {

	//! Current hotspot temperature.

	temperature hotspot_temperature_thread_only;

	//! Current activation callback

	refptr_traits<hotspot_callback_t>::ptr_t callback_thread_only;

	//! Whether ENTER or SPACE pressed.
	bool is_key_down=false;

	//! Whether button 1 is pressed.
	bool is_button1_down=false;
 public:
	THREAD_DATA_ONLY(hotspot_temperature);
	THREAD_DATA_ONLY(callback);

	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	//! Must be implemented in the subclass.

	//! Returns this hotspot's focusable.
	//! \see hotspot_element

	virtual focusableObj::implObj &get_hotspot_focusable()=0;

	//! Must be implemented in the subclass.

	//! Returns this hotspot's focusable.
	//! \see hotspot_element

	virtual const focusableObj::implObj &get_hotspot_focusable() const=0;

	//! Must be implemented in the subclass.

	//! Returns this hotspot's focusable's element.
	//! \see hotspot_element

	virtual child_elementObj &get_hotspot_element()=0;

	//! Must be implemented in the subclass.

	//! Returns this hotspot's focusable's element.
	//! \see hotspot_element

	virtual const child_elementObj &get_hotspot_element() const=0;

	//! Implement get_window_handler().

	//! Implements in terms of get_hotspot_element();

	generic_windowObj::handlerObj &get_window_handler();

	//! Update hotspot temperature due to keyboard focus change.

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger);

	//! Update hotspot temperature due to pointer focus change.

	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger);

	//! Update hotspot temperature due to a keyboard event.

	bool process_key_event(ONLY IN_THREAD, const key_event &ke);

	//! Update hotspot temperature to cold temperature.

	void grabbed_key_event(ONLY IN_THREAD);

	//! Update hotspot temperature due to the window focus change.

	void window_focus_change(ONLY IN_THREAD, bool flag);

	//! Button 1 press requests focus to us.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp);

 private:

	//! Update the current temperature

	void update(ONLY IN_THREAD,
		    const callback_trigger_t &trigger);

 public:

	//! Hotspot's temperature has changed.

	virtual void temperature_changed(ONLY IN_THREAD,
					 const callback_trigger_t &trigger);

	//! Install a new callback.

	void on_activate(const hotspot_callback_t &);

	//! Install a new callback.

	void on_activate(ONLY IN_THREAD, const hotspot_callback_t &);

	//! Whether hotspot should activate on this keypress.

	//! The default implementation detects space and Enter.

	virtual bool activate_on_key(const key_event &ke);

	//! The hotspot has been activated.

	//! The default implementation invokes callback().

	void activated(ONLY IN_THREAD, const callback_trigger_t &) override;

	//! Implement enabled().

	//! Implemented by forwarding it to the focusable.

	bool enabled(ONLY IN_THREAD) override;

	//! Set the keyboard shortcut for this hotspot.

	void set_shortcut(const shortcut &);

	virtual void install_shortcut(const shortcut &,
				      const activated_in_thread &)=0;

	virtual void uninstall_shortcut()=0;
};

LIBCXXW_NAMESPACE_END

#endif
