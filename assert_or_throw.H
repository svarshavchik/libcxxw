/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef assert_or_throw_H
#define assert_or_throw_H

#include "x/w/namespace.H"

//! This must be true. No exceptions.

#define assert_or_throw(v,t) do {					\
		if (!(v))						\
		{							\
			do_assert_or_throw(t, __FUNCTION__, __FILE__);	\
		}							\
	} while(0);

LIBCXXW_NAMESPACE_START

//! Throw an exception after an internal assertion failed.

void do_assert_or_throw(const char *error,
			const char *function,
			const char *file) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
