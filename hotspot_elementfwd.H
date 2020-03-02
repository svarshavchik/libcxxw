/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_elementfwd_h
#define hotspot_elementfwd_h

#include "hotspot.H"
#include "x/w/elementobj.H"
#include "x/w/input_mask.H"
#include "x/w/button_eventfwd.H"
#include "x/w/key_eventfwd.H"
#include "x/w/focus.H"
#include "x/w/impl/focus/focusable_elementfwd.H"
#include "shortcut/shortcut_activation_elementfwd.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
class LIBCXX_HIDDEN hotspot_elementObj;

//! A mixin for inheriting from a display element, and a hotspotObj::implObj.

//! The template parameter must be a subclass of child_elementObj.
//! This mixin alos requires the focusable_element mixin. A hotspot is
//! always focusable, because it handles keyboard and pointer clicks.
//!
//! This template class multiply-inherits from the
//! focusable_elementObj mixin,  and hotspotObj::implObj.

template<typename element_impl>
class hotspot_elementObj
	: public shortcut_activation_elementObj<focusable_elementObj
						<element_impl>>,
	  public hotspotObj::implObj {

	typedef focusable_elementObj<element_impl> nonshortcut_superclass_t;

	typedef shortcut_activation_elementObj<nonshortcut_superclass_t
					       > superclass_t;

 public:

	using superclass_t::superclass_t;

	~hotspot_elementObj();

	child_elementObj &get_hotspot_element() override;

	const child_elementObj &get_hotspot_element() const override;

	focusableObj::implObj &get_hotspot_focusable() override;

	const focusableObj::implObj &get_hotspot_focusable() const override;

	void keyboard_focus(ONLY IN_THREAD, const callback_trigger_t &)
		override;

	void pointer_focus(ONLY IN_THREAD,
			   const callback_trigger_t &trigger) override;

	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	void grabbed_key_event(ONLY IN_THREAD) override;

	void window_focus_change(ONLY IN_THREAD, bool flag) override;

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	void install_shortcut(const shortcut &,
			      const activated_in_thread &) override;

	void uninstall_shortcut() override;
};


LIBCXXW_NAMESPACE_END

#endif
