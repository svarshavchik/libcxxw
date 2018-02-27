/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "themeborder_element_impl.H"
#include "screen.H"
#include "defaulttheme.H"
#include "current_border_impl.H"
#include "element_screen.H"

LIBCXXW_NAMESPACE_START

themeborder_element_implObj::themeborder_element_implObj(const border_arg &arg,
							 elementObj::implObj &e)
	: current_border{e.get_screen()->impl->get_cached_border(arg)}
{
}

themeborder_element_implObj::~themeborder_element_implObj()=default;

void themeborder_element_implObj::initialize(IN_THREAD_ONLY)
{
	theme_updated(IN_THREAD,
		      get_border_element_impl().get_screen()->impl
		      ->current_theme.get());

	current_border->initialize(IN_THREAD);
}

void themeborder_element_implObj::theme_updated(IN_THREAD_ONLY,
						const defaulttheme &new_theme)
{
	current_border->theme_updated(IN_THREAD, new_theme);
}

LIBCXXW_NAMESPACE_END
