/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef date_input_field_impl_h
#define date_input_field_impl_h

#include "date_input_field/date_input_field_handler.H"
#include "date_input_field/date_input_field_calendarfwd.H"
#include "image_button_internalfwd.H"

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN date_input_fieldObj::implObj : virtual public obj {

 public:

	//! The actual implementation object handler.

	const ref<handlerObj> handler;

	//! The popup for this date input field.

	const popup calendar_popup;

	//! The container in the popup, with the currently shown month.

	const date_input_field_calendar calendar_container;

	//! The button that opens the popup
	const image_button_internal popup_imagebutton;

	//! Constructor
	implObj(const ref<handlerObj> &handler,
		const popup &calendar_popup,
		const date_input_field_calendar &calendar_container,
		const image_button_internal &popup_imagebutton);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif