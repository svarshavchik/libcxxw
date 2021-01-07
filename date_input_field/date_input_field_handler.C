/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "date_input_field/date_input_field_handler.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/nonrecursive_visibility.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

date_input_fieldObj::handlerObj
::handlerObj(const container_impl &parent_container)
	: superclass_t{parent_container}
{
}

date_input_fieldObj::handlerObj::~handlerObj()=default;

LIBCXXW_NAMESPACE_END
