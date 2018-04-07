/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_connection_threadfwd_h
#define x_w_impl_connection_threadfwd_h

#include <x/ptrfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

// A lightweight mechanism for enforcing that a particular class member
// can only be accessed by a connection thread. The class member's name is
// {name}_thread_only, and this declares name() that gets invoked as:
//
// name(IN_THREAD)

#define THREAD_DATA_ONLY(name)						\
	/*! This is accessible in connection thread only */		\
									\
	inline auto &name(ONLY IN_THREAD)				\
	{								\
		return name ## _thread_only;				\
	}								\
									\
	/*! This is accessible in connection thread only */		\
									\
	inline const auto &name(ONLY IN_THREAD) const			\
	{								\
		return name ## _thread_only;				\
	}								\


// Similar to THREAD_DATA_ONLY, but for a private superclass.

#define THREAD_DATA_SUPERCLASS_ONLY(superclass,name)			\
	/*! This is accessible in connection thread only */		\
									\
	inline superclass * name(ONLY IN_THREAD)			\
	{								\
		return this;						\
	}								\
									\
	/*! This is accessible in connection thread only */		\
									\
	inline const superclass * name(ONLY IN_THREAD) const		\
	{								\
		return this;				\
	}								\

LIBCXXW_NAMESPACE_END

#endif
