/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_busy_h
#define x_w_busy_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/ref.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Factory for busy mcguffins.

//! This is passed to callbacks invoked by the library's internal thread.
//! The internal thread resumes processing X events after the callback returns.
//! Invoking get_shade_mcguffin() or get_wait_mcguffin() returns an opaque
//! mcguffin whose existence blocks processing of pointer and key clicks until
//! the mcguffin gets destroyed.
//!
//! The execution thread ignores button and keyboard press events (but
//! not button and keyboard release events) until the last reference to the
//! mcguffin goes out of scope and it gets destroyed. So if the callback
//! retrieves the mcguffin and stores it somewhere, or includes it in a
//! message to the main application thread, the internal library thread
//! ignores button and keyboard press events until the last reference
//! to the mcguffin goes out of scope and it gets destroyed.
//!
//! The execution thread continues to invoke other callbacks that are
//! unrelated to keyboard or button activity, and they can also acquire
//! a mcguffin. This may return another reference to the same mcguffin,
//! and all references must go out of scope before the mcguffin gets destroyed.
//!
//! The pointer and button activity gets blocked only for the top level
//! application window from which the mcguffin was obtained. Each top level
//! application window has its own individual busy mcguffin.
//!
//! When a shade mcguffin exists, the top level application window gets drawn
//! with a dark shade (depending on the theme and the display's capabilities)
//! indicating that the application window is not processing events.
//!
//! When a wait mcguffin exists, the top level application window's cursor
//! gets changed to a "wait/busy" cursor pointer.
//!
//! There's also a get_shade_mcguffin() and get_wait_mcguffin() method
//! in \ref element "every widget", which obtain their respective
//! mcguffins as well.
//!
//! get_shade_busy_mcguffin() is equivalent to get_wait_busy_mcguffin() on
//! display screens that don't support composition with an alpha channel.

class busy {

public:
	//! Constructor
	busy() LIBCXX_HIDDEN;

	//! Destructor
	virtual ~busy() LIBCXX_HIDDEN;

	//! Copy constructor is deleted.
	busy(const busy &)=delete;

	//! Assignment operator is deleted.
	busy &operator=(const busy &)=delete;

	//! Acquire a busy mcguffin that puts a shade over the window.
	virtual ref<obj> get_shade_busy_mcguffin() const=0;

	//! Acquire a busy mcguffin that installs a "wait" pointer in the window.
	virtual ref<obj> get_wait_busy_mcguffin() const=0;
};

LIBCXXW_NAMESPACE_END

#endif
