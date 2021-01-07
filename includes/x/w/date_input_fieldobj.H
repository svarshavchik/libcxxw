/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_fieldobj_h
#define x_w_date_input_fieldobj_h

#include <x/w/date_input_fieldfwd.H>
#include <x/w/focusable_containerobj.H>
#include <x/ymd.H>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see date_input_field

class date_input_fieldObj : public focusable_containerObj {

public:
	class LIBCXX_HIDDEN implObj;

	class LIBCXX_HIDDEN handlerObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	date_input_fieldObj(const ref<implObj> &impl,
			    const layout_impl &my_layout_impl)
		LIBCXX_HIDDEN;

	//! Destructor
	~date_input_fieldObj() LIBCXX_HIDDEN;

	//! Inherited from focusableObj

	focusable_impl get_impl() const override LIBCXX_HIDDEN;

	//! Inherited from focusableObj

	void do_get_impl(const function<internal_focusable_cb> &cb)
		const override LIBCXX_HIDDEN;

	//! Set a new date in the field. Pass a null value to clear it.

	void set(const std::optional<ymd> &);

	//! Return the entered date. Returns a null value if no valid date is entered.

	std::optional<ymd> get() const;

	//! Install a callback that gets notified whenever the date changes.

	void on_change(const date_input_field_callback_t &);
};

LIBCXXW_NAMESPACE_END

#endif
