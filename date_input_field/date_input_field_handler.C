/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "date_input_field/date_input_field_handler.H"
#include "container_element.H"
#include "nonrecursive_visibility.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

date_input_fieldObj::handlerObj
::handlerObj(const ref<containerObj::implObj> &parent_container)
	: superclass_t{parent_container}
{
}

date_input_fieldObj::handlerObj::~handlerObj()=default;

LIBCXXW_NAMESPACE_END
