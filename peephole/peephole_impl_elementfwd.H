/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_impl_elementfwd_H
#define peephole_impl_elementfwd_H

#include "peephole/peephole_impl.H"
#include "x/w/button_eventfwd.H"
#include "x/w/containerobj.H"
#include <xcb/xcb.h>

LIBCXXW_NAMESPACE_START

template<typename superclass_t>
class LIBCXX_HIDDEN peephole_impl_elementObj;


//! Mixin for the \ref peepholeObj::implObj "peephole implementation object."

template<typename superclass_t>
class peephole_impl_elementObj : public superclass_t,
				 public peepholeObj::implObj {


public:
	using superclass_t::superclass_t;

	//! Destructor

	~peephole_impl_elementObj();

	//! Override process_button_event

	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp) override;

	//! Implement get_container_impl()

	containerObj::implObj &get_container_impl() override;
};

LIBCXXW_NAMESPACE_END

#endif
