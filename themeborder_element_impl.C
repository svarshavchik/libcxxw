/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/themeborder_element_impl.H"
#include "screen.H"
#include "defaulttheme.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/element.H"

LIBCXXW_NAMESPACE_START

themeborder_element_implObj::themeborder_element_implObj(const border_arg &arg,
							 elementObj::implObj &e)
	: current_border_thread_only{e.get_screen()->impl->get_cached_border(arg)}
{
}

themeborder_element_implObj::~themeborder_element_implObj()=default;

void themeborder_element_implObj::initialize(ONLY IN_THREAD)
{
	current_border(IN_THREAD)->initialize(IN_THREAD);
}

void themeborder_element_implObj::theme_updated(ONLY IN_THREAD,
						const defaulttheme &new_theme)
{
	current_border(IN_THREAD)->theme_updated(IN_THREAD, new_theme);
}

void themeborder_element_implObj::set_new_border(ONLY IN_THREAD, const border_arg &arg)
{
	current_border(IN_THREAD)=
		current_border(IN_THREAD)->screen->get_cached_border(arg);
	current_border(IN_THREAD)->initialize(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
