/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_calendarobj_h
#define x_w_date_input_field_calendarobj_h

#include "date_input_field/date_input_field_calendarfwd.H"
#include "peephole/peepholed_fontelementfwd.H"
#include "reference_font_elementfwd.H"
#include "peephole/peepholed_toplevel_elementfwd.H"
#include "always_visiblefwd.H"
#include "popup/popup_attachedto_infofwd.H"
#include "container_elementfwd.H"
#include "x/w/containerobj.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/input_fieldfwd.H"
#include "x/w/date_input_fieldfwd.H"
#include "child_element.H"

#include <x/ymd.H>
#include <x/mpobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see date_input_field_calendar

class LIBCXX_HIDDEN date_input_field_calendarObj
	: public peepholed_fontelementObj<peepholed_toplevel_elementObj
					  <containerObj>> {

	typedef peepholed_fontelementObj<peepholed_toplevel_elementObj
					 <containerObj>> superclass_t;
 public:

	//! Implementation object.

	typedef reference_font_elementObj<
		always_visibleObj<container_elementObj<child_elementObj>>
		> implObj;

	//! Constructor, phase 1.

	date_input_field_calendarObj(const popup_attachedto_info &,
				     const ref<implObj> &,
				     const ref<layoutmanagerObj::implObj> &,
				     const ymd &current_ym,
				     const input_field &text_input_field);

	//! Constructor, phase 2.
	void constructor(const popup_attachedto_info &,
			 const ref<implObj> &,
			 const ref<layoutmanagerObj::implObj> &,
			 const ymd &current_ym,
			 const input_field &text_input_field);

	//! Destructor
	~date_input_field_calendarObj();

 private:

	//! Attachment calculation info for the popup.
	const popup_attachedto_info attachedto_info;

 public:
	//! Input field for the date.
	const input_field text_input_field;
 private:
	//! Computed maximum width value.
	dim_t max_width_value;

	//! Computed maximum height value.
	dim_t max_height_value;

	//! Compute the maximum size.
	void recalculate_peepholed_metrics(IN_THREAD_ONLY,
					   const screen &s) override;

	//! Return the maximum width
	dim_t max_width(IN_THREAD_ONLY) const override;

	//! Return the maximum height
	dim_t max_height(IN_THREAD_ONLY) const override;

	//! Update the shown month.

	void update_month(mpobj<ymd>::lock &);

	//! Currently shown month.

	mpobj<ymd> current_ym;

	//! Most recently picked date+callback.

	struct most_recent_date_s {

		std::optional<ymd> date_value;
		date_input_field_callback_t callback;
	};

	//! Container for the most recently picked date+callback

	typedef mpobj<most_recent_date_s, std::recursive_mutex
		      > most_recent_date_t;

	//! Keep track of the most reecntly picked date+callback.

	most_recent_date_t most_recent_date;

 public:

	//! Invoke the callback, if one is installed.

	void report_new_date(const std::optional<ymd> &d,
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
	void picked(const ymd &y, const callback_trigger_t &callback);

	//! Set a new date in the field. Pass a null value to clear it.

	void set(const std::optional<ymd> &, const callback_trigger_t &);

	//! Return the entered date. Returns a null value if no valid date is entered.

	std::optional<ymd> get() const;

	//! Install a new callback.
	void on_change(const date_input_field_callback_t &);
};

LIBCXXW_NAMESPACE_END

#endif