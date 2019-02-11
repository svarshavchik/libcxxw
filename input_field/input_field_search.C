/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/input_field_search.H"
#include "input_field/input_field_search_popup_handler.H"
#include "popup/popup.H"

LIBCXXW_NAMESPACE_START

static inline child_element_init_params
create_init_params(const popup &my_popup)
{
	child_element_init_params init_params;

	init_params.attached_popup=my_popup;

	return init_params;
}

input_field_searchObj
::input_field_searchObj(const popup &my_popup,
			const ref<input_field_search_popup_handlerObj> &handler,
			const container_impl &parent)
	: superclass_t{parent, create_init_params(my_popup)},
	  my_popup{my_popup},
	  popup_handler{handler}
{
}

input_field_searchObj::~input_field_searchObj()=default;

LIBCXXW_NAMESPACE_END
