/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/input_field_search_popup_handlerobj.H"

LIBCXXW_NAMESPACE_START

input_field_search_popup_handlerObj::input_field_search_popup_handlerObj
(const peepholed_toplevel_listcontainer_handler_args &args)
	: superclass_t{args}
{
}

input_field_search_popup_handlerObj::~input_field_search_popup_handlerObj()
{
}

bool input_field_search_popup_handlerObj
::popup_accepts_key_events(ONLY IN_THREAD)
{
	return search_popup_activated(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
