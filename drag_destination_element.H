/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drag_destination_element_H
#define drag_destination_element_H

#include "drag_destination_elementfwd.H"

LIBCXXW_NAMESPACE_START

//! Implement find_acceptable_drop().

//! Calls this_element_is_an_acceptable_drop(), if not we forward this
//! call to its superclass.

template<typename element_impl>
bool drag_destination_elementObj<element_impl>
::find_acceptable_drop(ONLY IN_THREAD,
		       ref<elementObj::implObj> &accepting_element,
		       const source_dnd_formats_t &source_formats,
		       xcb_timestamp_t timestamp)
{
	if (this_element_accepts_this_drop(IN_THREAD,
					   accepting_element,
					   source_formats,
					   timestamp))
		return true;

	return superclass_t::find_acceptable_drop(IN_THREAD, accepting_element,
						  source_formats,
						  timestamp);
}

//! Invoke accepts_drop() if enabled().

template<typename element_impl>
bool drag_destination_elementObj<element_impl>
::this_element_accepts_this_drop(ONLY IN_THREAD,
				 ref<elementObj::implObj>
				 &accepting_element,
				 const source_dnd_formats_t &source_formats,
				 xcb_timestamp_t timestamp)
{
	if (this->enabled(IN_THREAD) &&
	    this->accepts_drop(IN_THREAD, source_formats, timestamp))
	{
		accepting_element=ref<elementObj::implObj>{this};
		return true;
	}
	return false;
}

LIBCXXW_NAMESPACE_END

#endif
