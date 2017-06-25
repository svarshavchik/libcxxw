/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "current_border_impl.H"
#include "border_impl.H"

LIBCXXW_NAMESPACE_START

current_border_implObj::current_border_implObj(const const_border_impl
					       &initial_border)
	: border_thread_only(initial_border)
{
}

current_border_implObj::~current_border_implObj()=default;

void current_border_implObj::theme_updated(IN_THREAD_ONLY,
					   const defaulttheme &new_theme)
{
}

LIBCXXW_NAMESPACE_END
