/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "date_input_field/date_input_field_impl.H"
#include "date_input_field/date_input_field_handler.H"
#include "date_input_field/date_input_field_calendar.H"
#include "popup/popup.H"
#include "image_button_internal.H"

LIBCXXW_NAMESPACE_START

date_input_fieldObj::implObj
::implObj(const ref<handlerObj> &handler,
	  const date_input_field_calendar &calendar_container,
	  const image_button_internal &popup_imagebutton)
	: handler{handler},
	  calendar_container{calendar_container},
	  popup_imagebutton{popup_imagebutton}
{
}

date_input_fieldObj::implObj::~implObj()=default;

LIBCXXW_NAMESPACE_END
