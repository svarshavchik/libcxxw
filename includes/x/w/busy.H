/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_busy_h
#define x_w_busy_h

#include <x/w/namespace.H>
#include <x/w/busyfwd.H>
#include <x/w/generic_windowobj.H>
#include <x/refcnt.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! Block processing of all key and button press event.

//! While any instance of this object exists in scope, somewhere, the
//! library ignores all key and button events.

class busy {

	//! Internal reference pointer.

	ref<generic_windowObj::handlerObj> w;

public:
	//! Constructor
	busy(const ref<generic_windowObj::handlerObj> &);

	//! Destructor
	~busy();

	//! Copy constructor
	busy(const busy &);

	//! Assignment operator is deleted.
	busy &operator=(const busy &)=delete;
};

LIBCXXW_NAMESPACE_END

#endif
