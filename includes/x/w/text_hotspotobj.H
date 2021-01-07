/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_hotspotobj_H
#define x_w_text_hotspotobj_H

#include <x/w/rgbfwd.H>
#include <x/w/text_paramfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/functionalrefptr.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Optional link, of some kind, from some portion of a rich text fragment.

class text_hotspotObj : virtual public obj {

public:

	//! Constructor
	text_hotspotObj(const functionref<text_param(THREAD_CALLBACK,
						     const text_event_t &)> &);

	//! Destructor
	~text_hotspotObj() noexcept;

	const functionref<text_param(THREAD_CALLBACK,
				     const text_event_t &)> event;
};

LIBCXXW_NAMESPACE_END

#endif
