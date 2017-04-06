/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_h
#define hotspot_h

#include "x/w/hotspotobj.H"
#include "x/w/hotspotfwd.H"
#include "x/w/elementobj.H"
#include "x/w/input_mask.H"
#include "x/w/focusablefwd.H"
#include "x/w/key_eventfwd.H"
#include "focus/focus.H"
#include "focus/focusable_elementfwd.H"
#include "connection_threadfwd.H"

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
				  int button,
				  bool press,
				  const input_mask &mask);
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
};

LIBCXXW_NAMESPACE_END

#endif
