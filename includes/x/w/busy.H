/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_busy_h
#define x_w_busy_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/ref.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Block processing of all key and button press event.

//! This is passed to callbacks invoked by the library's internal thread.
//! The internal thread resumes processing X events after the callback returns.
//! Invoking get_mcguffin() returns an opaque mcguffin.
//!
//! The execution thread ignores button and keyboard press events (but
//! not button and keyboard release events) until the last reference to the
//! mcguffin goes out of scope and it gets destroyed. So if the callback
//! retrieves the mcguffin and stores it somewhere, or includes it in a
//! message to the main application thread, the internal library thread
//! will ignore button and keyboard press events until the last reference
//! to the mcguffin goes out of scope and it gets destroyed.
//!
//! The execution thread may continue to invoke other callbacks that are
//! unrelated to keyboard or button activity, and those callbacks can also
//! get_mcguffin(). This returns another reference to the same mcguffin,
//! and all references must go out of scope before the mcguffin gets destroyed.
//!
//! The pointer and button activity is blocked only for the top level
//! application window from which the mcguffin was obtained. Each top level
//! application window has its own individual busy mcguffin.
//!
//! During mcguffin's existence, the top level application window gets drawn
//! with a dark shade (depending on the theme and the display's capabilities)
//! indicating that the application window is not processing events.
//!
//! Any display element's get_mcguffin() method also obtains the
//! busy mcguffin, independently.
//!
//! This object also serves as a reference handle to the internal
//! connection thread. Several public object methods require a busy handle
//! as one of their parameters. Since a reference to the busy object is
//! only passed to a callback from the internal library execution thread this
//! ensures that only the callbacks can invoke those methods.

class busy {

public:
	//! Constructor
	busy(const connection_thread &thread) LIBCXX_HIDDEN;

	//! Destructor
	virtual ~busy() LIBCXX_HIDDEN;

	//! Internal connection thread.
	const connection_thread thread;

	//! Copy constructor is deleted.
	busy(const busy &)=delete;

	//! Assignment operator is deleted.
	busy &operator=(const busy &)=delete;

	virtual x::ref<x::obj> get_mcguffin() const=0;
};

LIBCXXW_NAMESPACE_END

#endif
