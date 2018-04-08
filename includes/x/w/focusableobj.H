/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusableobj_h
#define x_w_focusableobj_h

#include <x/w/focusablefwd.H>
#include <x/w/focus.H>
#include <x/w/key_event.H>
#include <x/obj.H>
#include <x/functional.H>
#include <x/functionalrefptr.H>

#include <vector>

LIBCXXW_NAMESPACE_START

//! Internal group of focusable fields treated as a single focusable unit.

struct LIBCXX_HIDDEN internal_focusable_group;

//! Callback for processing internal_focusable_group.

typedef void internal_focusable_cb(const internal_focusable_group &);

//! Methods common to all display elements that can receive input focus.

//! \see focusable

class focusableObj : virtual public obj {

public:
	class implObj;
	class ownerObj;

	//! Constructor
	focusableObj() LIBCXX_HIDDEN;

	//! Destructor
	~focusableObj() LIBCXX_HIDDEN;

	//! Return the implementation object.

	//! \internal
	virtual ref<focusableObj::implObj> get_impl() const LIBCXX_HIDDEN=0;


	//! Enable/disable the focusable object.

	void set_enabled(bool);

	//! This focusable should be tabbed to before the other one.

	void get_focus_before(const focusable &other);

	//! This focusable should be tabbed to after the other one

	void get_focus_after(const focusable &other);

	//! These focusables should be tabbed to after me.
	void get_focus_after_me(const std::vector<focusable> &others);

	//! These focusables should be tabbed to before me.
	void get_focus_before_me(const std::vector<focusable> &others);

	//! This focusable should be the first tabbable focusable.
	void get_focus_first();

	//! Request for keyboard focus to be moved to this element.

	//! No action takes place if this focusable is disabled.
	void request_focus();

	//! Return the set of real internal focusable element implementations.

	//! \internal
	//!
	//! A focusable can actually comprise of multiple internal
	//! focusable implementations. This assembles a list of focusable
	//! elements that comprise this "master" focusable element, and
	//! arranges them in consecutive focus order, invoking the callback
	//! and passing this information.

	template<typename functor>
	void get_impl(functor &&f) const LIBCXX_HIDDEN;

	//! Type-erased get_impl().

	//! \internal
	virtual void do_get_impl(const function<internal_focusable_cb> &)
		const LIBCXX_HIDDEN;

	//! Install a new callback to report focus changes.

	//! The callback gets invoked upon installation, reporting this
	//! display element's current status.

	void on_keyboard_focus(const functionref<focus_callback_t> &)
		LIBCXX_PUBLIC;

	//! Install a new callback to report key events.

	void on_key_event(const functionref<key_event_callback_t> &)
		LIBCXX_PUBLIC;

	//! Give this element the initial input focus

	//! After the window/dialog opens, the input focus is placed into
	//! the first element with autofocus set.
	//!
	//! autofocus is set by default in input_fields only. Some specific
	//! dialogs may set autofocus for their own fields/

	void autofocus(bool);
};

template<typename functor>
inline void focusableObj::get_impl(functor &&f) const
{
	do_get_impl(make_function<internal_focusable_cb>
		    (std::forward<functor>(f)));
}

LIBCXXW_NAMESPACE_END

#endif
