/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef sync_counter_h
#define sync_counter_h

#include "xid_t.H"
#include "x/w/connection_threadfwd.H"
#include <xcb/sync.h>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN sync_counter;

//! Sync X extension, a counter.

class sync_counter : public new_xid {

public:

	//! Constructor
	sync_counter(ONLY IN_THREAD);

	//! Destructor
	~sync_counter();

	//! Counter ID
	inline xcb_sync_counter_t id() const { return id_; }

	//! Set counter

	void set(ONLY IN_THREAD, int64_t new_value);
};

LIBCXXW_NAMESPACE_END

#endif
