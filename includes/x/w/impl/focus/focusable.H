/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusable_h
#define x_w_impl_focusable_h

#include <x/w/focusableobj.H>
#include <x/w/elementobj.H>
#include <x/w/impl/focus/focusablefwd.H>
#include <x/w/impl/focus/label_forfwd.H>
#include <x/w/impl/connection_threadfwd.H>
#include <array>
#include <utility>
#include <x/weaklist.H>

LIBCXXW_NAMESPACE_START

class child_elementObj;

//! Return true if this key event is for advancing focus to the next field.

bool is_next_key(const key_event &) LIBCXX_HIDDEN;

//! Return true if keypress, and next_key().

bool next_key_pressed(const key_event &) LIBCXX_HIDDEN;

//! Return true if this key event is for advancing focus to the previous field.

bool is_prev_key(const key_event &) LIBCXX_HIDDEN;

//! Return true if keypress, and prev_key().

bool prev_key_pressed(const key_event &) LIBCXX_HIDDEN;

//! Return true if this key event is for advancing focus to the next page.

bool next_page_key(const key_event &) LIBCXX_HIDDEN;

//! Return true if keypress, and next_page_key().
bool next_page_key_pressed(const key_event &) LIBCXX_HIDDEN;

//! Return true if this key event is for advancing focus to the previous page.

bool prev_page_key(const key_event &) LIBCXX_HIDDEN;

//! Return true if keypress, and prev_page_key().
bool prev_page_key_pressed(const key_event &) LIBCXX_HIDDEN;

//! Return true if this key event is for selecting something.

bool select_key(const key_event &) LIBCXX_HIDDEN;

//! Return true if keypress, and select_key().
bool select_key_pressed(const key_event &) LIBCXX_HIDDEN;

//! Focusable field implementation object.

//! The focusable_elementObj template inherits from this class and a subclass
//! of child_elementObj, and implements get_focusable_element().
//!
//! The public object must inherit from focusableObj::ownerObj, which takes
//! care of invoking focusable_initialize() and focusable_deinitialize().

class focusableObj::implObj : virtual public obj {

	//! My iterator in the focusable_fields list.

	//! The public object's constructor and destructor is responsible
	//! for inserting me into the list.
	//!
	//! \note not valid unless in_focusable_fields is set.

	focusable_fields_t::iterator focusable_fields_iter_thread_only;

	//! Whether this object is now fully constructed.
	bool in_focusable_fields_thread_only;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// NOTE: USE GET_FOCUSABLE_FIELDS_ITER macro to access the above two!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//! Container of all labels for this focusable.
	typedef weaklist<label_forObj> my_labels_t;

	//! All labels for this focusable.

	my_labels_t my_labels_thread_only;

 public:

	THREAD_DATA_ONLY(focusable_fields_iter);
	THREAD_DATA_ONLY(in_focusable_fields);
	THREAD_DATA_ONLY(my_labels);

	//! Candidate for receiving focus automatically when shown.

	mpobj<bool> autofocus;

	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	//! Whether this widget is enabled for focusing.

	bool focusable_enabled(ONLY IN_THREAD);

	//! Set enabled/disabled

	void set_enabled(ONLY IN_THREAD, bool flag);

	//! Must be implement in the subclass.

	//! Returns this focusable's widget.
	//! \see focusable_element

	virtual child_elementObj &get_focusable_element()=0;

	//! Must be implement in the subclass.

	//! Returns this focusable's widget.
	//! \see focusable_element

	virtual const child_elementObj &get_focusable_element() const=0;

	//! Called from the public object's constructor to finish construction.

	//! Inserts itself into the focusable_fields list.
	void focusable_initialize(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Invoke from the public object's destructor

	void focusable_deinitialize(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Return the focusable fields.
	focusable_fields_t &focusable_fields(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! Advance input focus to the next focusable field.
	void next_focus(ONLY IN_THREAD,
			const callback_trigger_t &trigger) LIBCXX_HIDDEN;

	//! This is a label for this widget.
	void set_label(const ref<elementObj::implObj> &) LIBCXX_HIDDEN;

 private:
	//! Advance input focus to the next focusable field, on or after this iterator.

	//! The public object's destructor invokes next_focus() after removing
	//! focusable_fields_iter from focusable_fields.

	void next_focus(ONLY IN_THREAD,
			focusable_fields_t::iterator starting_iter,
			const callback_trigger_t &trigger) LIBCXX_HIDDEN;
 public:
	//! Advance input focus to the previous focusable field.
	void prev_focus(ONLY IN_THREAD,
			const callback_trigger_t &trigger) LIBCXX_HIDDEN;

	//! Set focus to this focusable widget.

	//! Sets the focus, but doesn't bother to call ensure_visibility().
	//! Typically done in response to a button press, because unexpected
	//! scrolling as a result of pressing a button would be a rather rude
	//! thing to do.

	void set_focus_only(ONLY IN_THREAD,
			    const callback_trigger_t &);

	//! If this focusable is enabled, then set_focus_and_ensure_visibility()

	//! Don't gripe if this focusable is not eligible for keyboard focus,
	//! just don't do it, if ok_if_not_possible is requested.
	//!
	//! Returns true if focus has been moved.
	bool request_focus_if_possible(ONLY IN_THREAD,
				       bool ok_if_not_possible);

	//! Whether this field is eligible to have its focus restored.

	//! When a popup gets closed, the last field with the input focus
	//! gets request_focusable().

	virtual bool focus_autorestorable(ONLY IN_THREAD);

	//! Set focus to this focusable widget and make sure it is visible.

	virtual void set_focus_and_ensure_visibility(ONLY IN_THREAD,
						     const callback_trigger_t &)
		;

 private:

	//! Called to move input focus to another focusable field.

	//! Invokes focus_to's set_focus_and_ensure_visibility().

	void switch_focus(ONLY IN_THREAD, const focusable_impl &focus_to,
			  const callback_trigger_t &trigger) LIBCXX_HIDDEN;

 public:
	//! Whether we're cool with giving up focus.
	virtual bool ok_to_lose_focus(ONLY IN_THREAD,
				      const callback_trigger_t &trigger);

	//! Set tabbing order.
	void get_focus_before(ONLY IN_THREAD,
			      const focusable_impl &other) LIBCXX_HIDDEN;
	//! Set tabbing order.
	void get_focus_after(ONLY IN_THREAD,
			      const focusable_impl &other) LIBCXX_HIDDEN;

 private:
	//! Set tabbing order.
	void i_will_get_focus_before(ONLY IN_THREAD,
				     focusableObj::implObj &other)
		LIBCXX_HIDDEN;
	//! Set tabbing order.
	void i_will_get_focus_after(ONLY IN_THREAD,
				    focusableObj::implObj &other)
		LIBCXX_HIDDEN;

 public:

	//! Throw an exception

	//! Invoked by the focusable_elementObj mixin when a child focusable
	//! widget gets created.
	//!
	//! Throw an exception, can't do it.

	void creating_focusable_child_element();

	//! Make sure we do not have input focus
	void unfocus(ONLY IN_THREAD) LIBCXX_HIDDEN;

 protected:

	//! Schedule unfocus() to be invoked later.

	//! This is called when the focusable widget is no longer visible.
	//! We don't want to unfocus() immediately, since other elements may
	//! also be losing visibility at the same time, so let's not bounce
	//! the focus all over the place. Instead punt this into idle
	//! callback processing.

	void unfocus_later(ONLY IN_THREAD);
};

void get_focus_before_in_thread(ONLY IN_THREAD,	const focusable &me,
				const focusable &other) LIBCXX_HIDDEN;

void get_focus_after_in_thread(ONLY IN_THREAD,	const focusable &me,
				const focusable &other) LIBCXX_HIDDEN;

void get_focus_impl_after_in_thread(ONLY IN_THREAD, const focusable &me,
				    const focusable_impl &a)
	LIBCXX_HIDDEN;

//! focusableObj::do_get_impl() constructs this object and invokes the
//! callback.

struct LIBCXX_HIDDEN internal_focusable_group {

	//! How many internal focusables there are in this group.
	size_t internal_impl_count;

	//! Pointer to an array of intenral_impl_count focusable implementation objects.
	focusable_impl *impls;
};

//! Fixed list of focusable elements in a single group.

//! This is used by an focusableObj::do_get_impl() implementation to
//! invoke its callback, passing it a fixed list of focusable implementation
//! objects.

template<typename ...Args>
inline void process_focusable_impls(const function<internal_focusable_cb> &cb,
				    Args && ...args) LIBCXX_HIDDEN;

template<typename ...Args>
inline void process_focusable_impls(const function<internal_focusable_cb> &cb,
				    Args && ...args)
{
	std::array<focusable_impl, sizeof...(Args)> impls={
		std::forward<Args>(args)...};

	internal_focusable_group group{impls.size(), &impls[0]};

	cb(group);
}

//! Take a vector of focusables, extract their implementations, invoke the callback.

//! Note, this individually grabs all impls from do_get_impl(), and returns
//! from it. Handle with care. Make sure all locks are in place.

void process_focusable_impls_from_focusables
(const function<internal_focusable_cb> &cb,
 const std::vector<focusable> &v)
	LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
