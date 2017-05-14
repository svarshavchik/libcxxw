/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusable_h
#define focusable_h

#include "x/w/focusableobj.H"
#include "x/w/elementobj.H"
#include "focus/focusablefwd.H"
#include "focus/label_forfwd.H"
#include "connection_threadfwd.H"

#include <x/weaklist.H>

LIBCXXW_NAMESPACE_START

class child_elementObj;

//! Focusable field implementation object.

//! The focusable_elementObj template inherits from this class and a subclass
//! of child_elementObj, and implements get_focusable_element().
//!
//! The public object must inherit from focusableObj::ownerObj, which takes
//! care of invoking focusable_initialize() and focusable_deinitialize().

class LIBCXX_HIDDEN focusableImplObj : virtual public obj {

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

	//! Constructor
	focusableImplObj();

	//! Destructor
	~focusableImplObj();

	//! Whether this element is enabled for focusing.

	bool enabled(IN_THREAD_ONLY);

	//! Set enabled/disabled

	void set_enabled(IN_THREAD_ONLY, bool flag);

	//! Must be implement in the subclass.

	//! Returns this focusable's element.
	//! \see focusable_element

	virtual child_elementObj &get_focusable_element()=0;

	//! Must be implement in the subclass.

	//! Returns this focusable's element.
	//! \see focusable_element

	virtual const child_elementObj &get_focusable_element() const=0;

	//! Called from the public object's constructor to finish construction.

	//! Inserts itself into the focusable_fields list.
	void focusable_initialize(IN_THREAD_ONLY);

	//! Invoke from the public object's destructor

	void focusable_deinitialize(IN_THREAD_ONLY);

	//! Return the focusable fields.
	focusable_fields_t &focusable_fields(IN_THREAD_ONLY);

	//! Advance input focus to the next focusable field.
	void next_focus(IN_THREAD_ONLY);

	//! This is a label for this display element.
	void set_label(const ref<elementObj::implObj> &);

 private:
	//! Advance input focus to the next focusable field, on or after this iterator.

	//! The public object's destructor invokes next_focus() after removing
	//! focusable_fields_iter from focusable_fields.

	void next_focus(IN_THREAD_ONLY,
			focusable_fields_t::iterator starting_iter);
 public:
	//! Advance input focus to the previous focusable field.
	void prev_focus(IN_THREAD_ONLY);

	//! Set focus to this focusable element.
	void set_focus(IN_THREAD_ONLY);

 private:

	//! Called to move input focus to another focusable field.
	void switch_focus(IN_THREAD_ONLY, const focusable_impl &focus_to);

 public:
	//! Set tabbing order.
	void get_focus_before(IN_THREAD_ONLY,
			      const ref<focusableImplObj> &other);
	//! Set tabbing order.
	void get_focus_after(IN_THREAD_ONLY,
			      const ref<focusableImplObj> &other);

 private:
	//! Set tabbing order.
	void i_will_get_focus_before(IN_THREAD_ONLY,
				     focusableImplObj &other);
	//! Set tabbing order.
	void i_will_get_focus_after(IN_THREAD_ONLY,
				    focusableImplObj &other);

};

LIBCXXW_NAMESPACE_END

#endif
