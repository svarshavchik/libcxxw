/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/input_field_search_popup_handlerobj.H"
#include "popup/popup_attachedto_info.H"

LIBCXXW_NAMESPACE_START

input_field_search_popup_handlerObj::input_field_search_popup_handlerObj
(const peepholed_toplevel_listcontainer_handler_args &args)
	: superclass_t{args}
{
}

input_field_search_popup_handlerObj::~input_field_search_popup_handlerObj()
{
}

popup_position_affinity input_field_search_popup_handlerObj
::recalculate_attached_popup_position(ONLY IN_THREAD,
			     rectangle &r,
			     dim_t screen_width,
			     dim_t screen_height)
{
	auto &attachedto_element_position=
		attachedto_info->attachedto_element_position(IN_THREAD);

	r.width=attachedto_element_position.width;
	return superclass_t::recalculate_attached_popup_position(IN_THREAD, r,
							screen_width,
							screen_height);
}

bool input_field_search_popup_handlerObj
::popup_accepts_key_events(ONLY IN_THREAD)
{
	return search_popup_activated(IN_THREAD);
}

void input_field_search_popup_handlerObj
::set_inherited_visibility_unmapped(ONLY IN_THREAD)
{
	search_popup_activated(IN_THREAD)=false;
	superclass_t::set_inherited_visibility_unmapped(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
