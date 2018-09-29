/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_calendarobj_h
#define x_w_date_input_field_calendarobj_h

#include "date_input_field/date_input_field_calendarfwd.H"
#include "peephole/peepholed_attachedto_containerobj.H"
#include "x/w/containerobj.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/input_fieldfwd.H"
#include "x/w/date_input_fieldfwd.H"

#include <x/ymd.H>
#include <x/mpobj.H>
#include <x/refptr_traits.H>
#include <x/functionalrefptr.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see date_input_field_calendar

class date_input_field_calendarObj
	: public peepholed_attachedto_containerObj {

 public:

	//! Constructor, phase 1.

	date_input_field_calendarObj(const popup_attachedto_info &,
				     const ref<implObj> &,
				     const layout_impl &,
				     const ymd &current_ym,
				     const input_field &text_input_field);

	//! Constructor, phase 2.
	void constructor(const popup_attachedto_info &,
			 const ref<implObj> &,
			 const layout_impl &,
			 const ymd &current_ym,
			 const input_field &text_input_field);

	//! Destructor
	~date_input_field_calendarObj();

 public:
	//! Input field for the date.
	const input_field text_input_field;
 private:

	//! Update the shown month.

	void update_month(mpobj<ymd>::lock &);

	//! Currently shown month.

	mpobj<ymd> current_ym;

	//! Most recently picked date+callback.

	struct most_recent_date_s {

		std::optional<ymd> date_value;
		refptr_traits<date_input_field_callback_t>::ptr_t callback;
	};

	//! Container for the most recently picked date+callback

	typedef mpobj<most_recent_date_s, std::recursive_mutex
		      > most_recent_date_t;

	//! Keep track of the most reecntly picked date+callback.

	most_recent_date_t most_recent_date;

 public:

	//! Invoke the callback, if one is installed.

	void report_new_date(THREAD_CALLBACK,
			     const std::optional<ymd> &d,
			     const callback_trigger_t &trigger);

	// Switch to a new calendar.

	void update_month(const ymd &new_month);

	//! Show previous year
	void prev_year();

	//! Show previous month
	void prev_mon();

	//! Show next year
	void next_year();

	//! Show next month
	void next_mon();

	//! Picked a date.
	void picked(THREAD_CALLBACK,
		    const ymd &y, const callback_trigger_t &callback);

	//! Set a new date in the field. Pass a null value to clear it.

	void set(THREAD_CALLBACK,
		 const std::optional<ymd> &, const callback_trigger_t &);

	//! Return the entered date. Returns a null value if no valid date is entered.

	std::optional<ymd> get() const;

	//! Install a new callback.
	void on_change(const date_input_field_callback_t &);
};

LIBCXXW_NAMESPACE_END

#endif
