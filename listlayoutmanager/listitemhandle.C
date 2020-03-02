/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listitemhandle_impl.H"
#include "x/w/uielements.H"
#include "messages.H"

LIBCXXW_NAMESPACE_START

listitemhandleObj::listitemhandleObj()=default;

listitemhandleObj::~listitemhandleObj()=default;


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
