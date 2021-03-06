/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/themeborder_element_impl.H"
#include "screen.H"
#include "defaulttheme.H"
#include "generic_window_handler.H"
#include "x/w/impl/current_border_impl.H"
#include "x/w/impl/element.H"

LIBCXXW_NAMESPACE_START

themeborder_element_implObj::themeborder_element_implObj(const border_arg &arg,
							 elementObj::implObj &e)
	: current_border_thread_only{e.create_border(arg)}
{
}

themeborder_element_implObj::~themeborder_element_implObj()=default;

void themeborder_element_implObj::set_new_border(ONLY IN_THREAD,
						 const border_arg &arg)
{
	current_border(IN_THREAD)=get_border_element_impl().create_border(arg);
}

LIBCXXW_NAMESPACE_END
