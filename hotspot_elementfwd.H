/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef hotspot_elementfwd_h
#define hotspot_elementfwd_h

#include "hotspot.H"
#include "x/w/elementobj.H"
#include "x/w/input_mask.H"
#include "x/w/key_eventfwd.H"
#include "focus/focus.H"
#include "focus/focusable_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! A mixin for inherting from a display element, and a hotspotObj::implObj.

//! The template parameter must be a subclass of child_elementObj.
//! This mixin alos requires the focusable_element mixin. A hotspot is
//! always focusable, because it handles keyboard and pointer clicks.
//!
//! This template class multiply-inherits from the
//! focusable_elementObj mixin,  and hotspotObj::implObj.

template<typename element_impl=child_elementObj>
class LIBCXX_HIDDEN hotspot_elementObj
	: public focusable_elementObj<element_impl>,
	  public hotspotObj::implObj {

 public:

	using focusable_elementObj<element_impl>::focusable_elementObj;

	~hotspot_elementObj();

	child_elementObj &get_hotspot_element() override;

	const child_elementObj &get_hotspot_element() const override;

	focusableImplObj &get_hotspot_focusable() override;

	const focusableImplObj &get_hotspot_focusable() const override;

	void keyboard_focus(IN_THREAD_ONLY,
			    focus_change event,
			    const ref<elementObj::implObj> &ptr) override;

	void pointer_focus(IN_THREAD_ONLY,
			   focus_change event,
			   const ref<elementObj::implObj> &ptr) override;

	bool process_key_event(IN_THREAD_ONLY, const key_event &ke) override;
	bool process_button_event(IN_THREAD_ONLY,
				  int button,
				  bool press,
				  const input_mask &mask) override;
};


LIBCXXW_NAMESPACE_END

#endif
