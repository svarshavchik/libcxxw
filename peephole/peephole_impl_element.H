/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_impl_element_H
#define peephole_impl_element_H

#include "peephole/peephole_impl_elementfwd.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

template<typename superclass_t>
peephole_impl_elementObj<superclass_t>::~peephole_impl_elementObj()=default;

template<typename superclass_t>
bool peephole_impl_elementObj<superclass_t>
::process_button_event(ONLY IN_THREAD,
		       const button_event &be,
		       xcb_timestamp_t timestamp)
{
	if (peepholeObj::implObj::process_button_event(IN_THREAD, be,
						       timestamp))
		return true;

	return superclass_t::process_button_event(IN_THREAD, be, timestamp);
}

template<typename superclass_t>

containerObj::implObj &peephole_impl_elementObj<superclass_t>
::get_container_impl()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif
