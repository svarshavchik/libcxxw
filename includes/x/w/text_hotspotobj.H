/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_text_hotspotobj_H
#define x_w_text_hotspotobj_H

#include <x/obj.H>
#include <x/w/rgbfwd.H>
#include <x/w/text_paramfwd.H>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Optional link, of some kind, from some portion of a rich text fragment.

class text_hotspotObj : virtual public obj {

public:

	//! Constructor
	text_hotspotObj(const std::function<text_param(const text_event_t &)> &)
		;

	//! Destructor
	~text_hotspotObj() noexcept;

	const std::function<text_param(const text_event_t &)> event;
};

LIBCXXW_NAMESPACE_END

#endif
