/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_h
#define hotspot_h

#include "hotspotfwd.H"
#include "x/w/elementobj.H"
#include "x/w/input_mask.H"
#include "x/w/focusablefwd.H"
#include "x/w/key_eventfwd.H"
#include "x/w/button_eventfwd.H"
#include "x/w/shortcut.H"
#include "x/w/focus.H"
#include "focus/focusable_elementfwd.H"
#include "connection_threadfwd.H"

#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

class child_elementObj;

//! Hotspot implementation.

//! The hotspot_elementObj template inherits from this class and a subclass
//! of child_elementObj, and implements get_hotspot_element().

class LIBCXX_HIDDEN hotspotObj::implObj : virtual public obj {

	//! Current hotspot temperature.

	temperature hotspot_temperature_thread_only;

	//! Current activation callback

	hotspot_callback_t callback_thread_only;

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

	//! Invoked from the public object's destructor

	//! Uninstalls any currently-installed shortcut
	void hotspot_deinitialize(IN_THREAD_ONLY);

	//! Must be implement in the subclass.

	//! Returns this hotspot's focusable.
	//! \see hotspot_element

	virtual focusableImplObj &get_hotspot_focusable()=0;

	//! Must be implement in the subclass.

	//! Returns this hotspot's focusable.
	//! \see hotspot_element

	virtual const focusableImplObj &get_hotspot_focusable() const=0;

	//! Must be implement in the subclass.

	//! Returns this hotspot's focusable's element.
	//! \see hotspot_element

	virtual child_elementObj &get_hotspot_element()=0;

	//! Must be implement in the subclass.

	//! Returns this hotspot's focusable's element.
	//! \see hotspot_element

	virtual const child_elementObj &get_hotspot_element() const=0;

	//! Update hotspot temperature due to keyboard focus change.

	void keyboard_focus(IN_THREAD_ONLY,
			    focus_change event,
			    const ref<elementObj::implObj> &ptr);

	//! Update hotspot temperature due to pointer focus change.

	void pointer_focus(IN_THREAD_ONLY,
			   focus_change event,
			   const ref<elementObj::implObj> &ptr);

	//! Update hotspot temperature due to a keyboard event.

	bool process_key_event(IN_THREAD_ONLY, const key_event &ke);

	//! Button 1 press requests focus to us.

	bool process_button_event(IN_THREAD_ONLY,
				  const button_event &be,
				  xcb_timestamp_t timestamp);
 private:

	//! Update the current temperature

	void update(IN_THREAD_ONLY);

 public:

	//! Hotspot's temperature has changed.

	virtual void temperature_changed(IN_THREAD_ONLY);

	//! Install a new callback.

	void on_activate(const hotspot_callback_t &);

	//! Install a new callback.

	void on_activate(IN_THREAD_ONLY, const hotspot_callback_t &);

	//! The hotspot has been activated.

	//! The default implementation invokes callback().

	virtual void activated(IN_THREAD_ONLY);

 private:

	//! The shortcut for this hotspot
	shortcut hotspot_shortcut;

	//! When shortcut is non-empty, it's installed a shortcut_lookup_t.

	shortcut_lookup_t::iterator installed_hotspot_iter;

 public:

	//! Set the keyboard shortcut for this hotspot.

	void set_shortcut(IN_THREAD_ONLY, const shortcut &);

	//! Get the keyboard shortcut.

	inline const shortcut &get_shortcut(IN_THREAD_ONLY)
	{
		return hotspot_shortcut;
	}
};

LIBCXXW_NAMESPACE_END

#endif
