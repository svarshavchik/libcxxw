/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_date_input_fieldfwd_h
#define x_w_date_input_fieldfwd_h

#include <x/w/namespace.H>
#include <x/w/callback_triggerfwd.H>
#include <x/ptrfwd.H>
#include <x/ymd.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

class date_input_fieldObj;

/*! A date input field.

A text input field with a button that opens a monthly calendar popup.
Selecting a day from a popup fills the text input field with the calendar
date. A calendar date may be directly typed in.

\code
#include <INSERT_LIBX_NAMESPACE/ymd.H>

field->set(INSERT_LIBX_NAMESPACE::ymd{2018, 1, 1});

field->set(std::nullopt);

std::optional<INSERT_LIBX_NAMESPACE::ymd> d=field->get();
\endcode

The <a href="/intervals.html">INSERT_LIBX_NAMESPACE::ymd</a> class
represents calendar dates. get() returns the current date, if there is one.
No value gets returned if there's no date entered, or the entered text does
not parse as a valid date. set() sets the date shown in the date input field,
or clears the field if the date is not provided.

\code
field->on_change(
    []
    (ONLY IN_THREAD,
     const std::optional<INSERT_LIBX_NAMESPACE::ymd> &new_date,
     const INSERT_LIBX_NAMESPACE::w::callback_trigger_t &trigger)
    {
    });
\endcode

on_change() installs a new callback that gets executed whenever a new
date is entered into the field. Its first parameter is the new date;
an empty \c new_date value indicates that the date input field is empty
or does not contain a valid date.

The \c trigger parameter indicates the reason the callback gets invoked.
The new callback gets immediately invoked upon installation to report the
current date in the field (if any), with the trigger set to
\c INSERT_LIBX_NAMESPACE::w::initial; then the trigger gets invoked whenever
the date input field's content change.

\note
The callback gets invoked by the library's internal execution thread.
The \ref initial "initial" callback invocation may occur before or after
on_change() returns.

*/

typedef ref<date_input_fieldObj> date_input_field;

//! A constant \ref date_input_field "date input field".

//! \see date_input_field

typedef const_ref<date_input_fieldObj> const_date_input_field;

//! A nullable pointer reference to a \ref date_input_field "date input field".

//! \see date_input_field

typedef ptr<date_input_fieldObj> date_input_fieldptr;

//! A nullable pointer reference to a const \ref date_input_field "date input field".

//! \see date_input_field

typedef const_ptr<date_input_fieldObj> const_date_input_fieldptr;

//! Callback that gets invoked when the date input field's value changes.

typedef functionref<void (THREAD_CALLBACK, const std::optional<ymd> &,
			  const callback_trigger_t &)
		    > date_input_field_callback_t;

LIBCXXW_NAMESPACE_END

#endif
