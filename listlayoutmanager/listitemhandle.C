/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listitemhandle_impl.H"
#include "x/w/uielements.H"
#include "x/w/callback_trigger.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

listitemhandleObj::listitemhandleObj()=default;

listitemhandleObj::~listitemhandleObj()=default;

void listitemhandleObj::selected(ONLY IN_THREAD, bool selected_flag)
{
	selected(IN_THREAD, selected_flag, {});
}

void listitemhandleObj::autoselect(ONLY IN_THREAD)
{
	autoselect(IN_THREAD, {});
}

listitemhandle uielements::get_listitemhandle(const std::string_view &name)
	const
{
	// TODO: C++20

	auto iter=new_list_item_handles.find(std::string{name.begin(),
								 name.end()});

	if (iter == new_list_item_handles.end())
		throw EXCEPTION(gettextmsg(_("List item \"%1%\" not found"),
					   name));

	return iter->second;
}

LIBCXXW_NAMESPACE_END
