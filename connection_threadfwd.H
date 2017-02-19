/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef connection_threadfwd_h
#define connection_threadfwd_h

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class connection_threadObj;

typedef ref<connection_threadObj> connection_thread;

typedef ptr<connection_threadObj> connection_threadptr;

/////////////////
//
// Functions and methods that can only be invoked by a connection thread
// take this as their first parameter.

#define IN_THREAD_ONLY const connection_thread &thread_

// And when one of them calls another one, it passes this as the firts
// parameter

#define IN_THREAD thread_

// A lightweight mechanism for enforcing that a particular class member
// can only be accessed by a connection thread. The class member's name is
// {name}_thread_only, and this declares name() that gets invoked as:
//
// name(IN_THREAD)

#define THREAD_DATA_ONLY(name)						\
	/*! This is accessible in connection thread only */		\
									\
	inline auto &name(IN_THREAD_ONLY)				\
	{								\
		return name ## _thread_only;				\
	}								\
									\
	/*! This is accessible in connection thread only */		\
									\
	inline const auto &name(IN_THREAD_ONLY) const			\
	{								\
		return name ## _thread_only;				\
	}								\


// Similar to THREAD_DATA_ONLY, but for a private superclass.

#define THREAD_DATA_SUPERCLASS_ONLY(superclass,name)			\
	/*! This is accessible in connection thread only */		\
									\
	inline superclass * name(IN_THREAD_ONLY)			\
	{								\
		return this;						\
	}								\
									\
	/*! This is accessible in connection thread only */		\
									\
	inline const superclass * name(IN_THREAD_ONLY) const		\
	{								\
		return this;				\
	}								\

LIBCXXW_NAMESPACE_END

#endif
