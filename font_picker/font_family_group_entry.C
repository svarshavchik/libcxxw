/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "font_picker/font_family_group_entry.H"

LIBCXXW_NAMESPACE_START

bool font_picker_group_id::operator<(const font_picker_group_id &o) const
{
	if (family < o.family)
		return true;
	if (o.family < family)
		return false;

	return foundry < o.foundry;
}

bool font_picker_group_id::operator==(const font_picker_group_id &o) const
{
	return family == o.family && foundry == o.foundry;
}

font_family_group_entryObj
::font_family_group_entryObj(const font_picker_group_id &id) : id{id},
							       name{id.family}
{
}

font_family_group_entryObj::~font_family_group_entryObj()=default;

LIBCXXW_NAMESPACE_END
