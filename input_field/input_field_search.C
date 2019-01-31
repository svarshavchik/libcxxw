/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/input_field_search.H"
#include "input_field/input_field_search_popup_handler.H"
#include "popup/popup.H"
#include "popup/popup_attachedto_handler_element.H"
#include "popup/popup_attachedto_handler.H"

LIBCXXW_NAMESPACE_START

input_field_searchObj
::input_field_searchObj(const popup &my_popup,
			const ref<input_field_search_popup_handlerObj> &handler,
			const container_impl &parent)
	: superclass_t{handler, parent},
	  my_popup{my_popup},
	  popup_handler{handler}
{
}

input_field_searchObj::~input_field_searchObj()=default;

LIBCXXW_NAMESPACE_END
