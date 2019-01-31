/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_impl_H
#define peephole_impl_H

#include "peephole/peepholeobj.H"
#include <xcb/xproto.h>

LIBCXXW_NAMESPACE_START

//! \ref peephole "peephole" implementation object.

//! Combined with the \ref peephole::elementObj mixin.

class peepholeObj::implObj : virtual public obj {

 public:

	//! Constructor.

	implObj();

	//! Destructor
	~implObj();

	//! Return my container.

	virtual containerObj::implObj &get_container_impl()=0;

	//! Scroll wheel in a peephole scrolls it vertically.

	//! Processes button 4 and 5 events.

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp);
};

LIBCXXW_NAMESPACE_END

#endif
