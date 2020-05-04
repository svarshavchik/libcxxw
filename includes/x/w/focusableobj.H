/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_focusableobj_h
#define x_w_focusableobj_h

#include <x/w/focusablefwd.H>
#include <x/w/focus.H>
#include <x/w/key_event.H>
#include <x/w/elementfwd.H>
#include <x/obj.H>
#include <x/functional.H>
#include <x/functionalrefptr.H>

#include <vector>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! Internal group of focusable fields treated as a single focusable unit.

struct LIBCXX_HIDDEN internal_focusable_group;

//! Callback for processing internal_focusable_group.

typedef void internal_focusable_cb(const internal_focusable_group &);

//! Methods common to all widgets that can receive input focus.

//! \see focusable

class focusableObj : virtual public obj {

public:
	class implObj;
	class ownerObj;

	//! Constructor
	focusableObj();

	//! Destructor
	~focusableObj();

	//! Return the implementation object.

	//! \internal
	virtual ref<implObj> get_impl() const=0;

	//! Enable/disable the focusable object.

	void set_enabled(bool);

	//! Enable/disable the focusable object.

	void set_enabled(ONLY IN_THREAD, bool);

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

	//! Request for keyboard focus to be moved to this widget.

	//! No action takes place if this focusable is currently disabled, and
	//! the focus move gets delayed until the widget becomes enabled.
	//! A subsequent explicit or implicit focus change will supercede
	//! the delayed action. An optional true parameter forces the focus
	//! change to occur immediately, if ever, and not delay it otherwise.
	void request_focus(bool now_or_never=false);

	//! This focusable should be tabbed to before the other one.

	void get_focus_before(ONLY IN_THREAD, const focusable &other);

	//! This focusable should be tabbed to after the other one

	void get_focus_after(ONLY IN_THREAD, const focusable &other);

	//! These focusables should be tabbed to after me.
	void get_focus_after_me(ONLY IN_THREAD,
				const std::vector<focusable> &others);

	//! These focusables should be tabbed to before me.
	void get_focus_before_me(ONLY IN_THREAD,
				 const std::vector<focusable> &others);

	//! This focusable should be the first tabbable focusable.
	void get_focus_first(ONLY IN_THREAD);

	//! Request for keyboard focus to be moved to this widget.

	//! No action takes place if this focusable is disabled. Returns
	//! true if focus was moved.
	void request_focus(ONLY IN_THREAD,
			   bool now_or_never=false);

	//! Return the set of real internal focusable widget implementations.

	//! \internal
	//!
	//! A focusable can actually comprise of multiple internal
	//! focusable implementations. This assembles a list of focusable
	//! elements that comprise this "master" focusable element, and
	//! arranges them in consecutive focus order, invoking the callback
	//! and passing this information.

	template<typename functor>
	void get_impl(functor &&f) const;

	//! Type-erased get_impl().

	//! \internal
	virtual void do_get_impl(const function<internal_focusable_cb> &)
		const LIBCXX_PUBLIC;

	//! Install a new callback to report focus changes.

	//! The callback gets invoked upon installation, reporting this
	//! widget's current status.

	void on_keyboard_focus(const functionref<focus_callback_t> &)
		LIBCXX_PUBLIC;

	//! Install a new callback to report key events.

	void on_key_event(const functionref<key_event_callback_t> &)
		LIBCXX_PUBLIC;

	//! Give this widget the initial input focus

	//! After the window/dialog opens, the input focus is placed into
	//! the first widget with autofocus set.
	//!
	//! autofocus is set by default in input_fields only. Some specific
	//! dialogs may set autofocus for their own fields/

	void autofocus(bool);

	//! Check a cut or copy operation if this focusable has keyboard focus.
	bool focusable_cut_or_copy_selection(cut_or_copy_op,
					     const std::string_view &selection);

	//! Check a cut or copy operation if this focusable has keyboard focus.

	//! The default selection clipboard is a configuration setting.
	bool focusable_cut_or_copy_selection(cut_or_copy_op);

	//! Check a cut or copy operation if this focusable has keyboard focus.
	bool focusable_cut_or_copy_selection(ONLY IN_THREAD,
					     cut_or_copy_op,
					     const std::string_view &selection);

	//! Check a cut or copy operation if this focusable has keyboard focus.

	//! The default selection clipboard is a configuration setting.
	bool focusable_cut_or_copy_selection(ONLY IN_THREAD,
					     cut_or_copy_op);

	//! Receive a selection, if this focusable has keyboard focus.

	//! Checks that this widget is currently receiving keyboard
	//! focus, if so then call's \ref generic_window "top level window"'s
	//! receive_selection().

	void focusable_receive_selection(const std::string_view &selection);

	//! Receive the default selection, if this focusable has keyboard focus.

	//! The default selection clipboard is a configuration setting.

	void focusable_receive_selection();

	//! Receive a selection, if this focusable has keyboard focus.

	//! Checks that this widget is currently receiving keyboard
	//! focus, if so then call's \ref generic_window "top level window"'s
	//! receive_selection().

	void focusable_receive_selection(ONLY IN_THREAD,
					 const std::string_view &selection);

	//! Receive the default selection, if this focusable has keyboard focus.

	//! The default selection clipboard is a configuration setting.

	void focusable_receive_selection(ONLY IN_THREAD);
};

template<typename functor>
inline void focusableObj::get_impl(functor &&f) const
{
	do_get_impl(make_function<internal_focusable_cb>
		    (std::forward<functor>(f)));
}

//! A focusable implementation mixin.

//! A widget that can received pointer or keyboard focus.
//!
//! There must be exactly one \ref focusableObj::ownerObj "owner object"
//! that officially owns a reference to the focusable object.
//!
//! A list of focusable fields is stored in the top level window handler
//! objects. This creates a circular reference, since widgets own
//! strong references to their parent widget, and the list becomes a
//! circular reference from the topmost widget to its child elements.
//!
//! The focusable owner object is responsible for creating the circular
//! reference in its constructor and removing it in the destructor.
//!
//! This focusable implementation object handles the mechanics of switching
//! keyboard input focus to the next/previous focusable object.

typedef ref<focusableObj::implObj> focusable_impl;

//! A nullable pointer reference to a \ref focusable_impl "focusable object".

typedef ptr<focusableObj::implObj> focusable_implptr;

LIBCXXW_NAMESPACE_END

#endif
