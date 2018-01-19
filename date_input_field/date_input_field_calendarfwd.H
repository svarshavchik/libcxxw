/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_field_calendarfwd_h
#define x_w_date_input_field_calendarfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class date_input_field_calendarObj;

/*! A calendar for a month.

The display element in the date input field's popup that actually displays
a calendar for one month.

This display element is peepholed by the popup's top-level peephole, and
thusly implements the peepholed mixin, using a reference font to set the
(mostly unused) top level scroll increments.

*/

typedef ref<date_input_field_calendarObj> date_input_field_calendar;

//! A constant \ref date_input_field_calendar "calendar month element".

//! \see date_input_field_calendar

typedef const_ref<date_input_field_calendarObj> const_date_input_field_calendar;

//! A nullable pointer reference to a \ref date_input_field_calendar "calendar month element".

//! \see date_input_field_calendar

typedef ptr<date_input_field_calendarObj> date_input_field_calendarptr;

//! A nullable pointer reference to a const \ref date_input_field_calendar "calendar month element".

//! \see date_input_field_calendar

typedef const_ptr<date_input_field_calendarObj> const_date_input_field_calendarptr;

LIBCXXW_NAMESPACE_END

#endif
