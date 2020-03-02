/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusablefwd_h
#define x_w_impl_focusablefwd_h

#include <x/ptrfwd.H>
#include <x/w/focusable.H>
#include <x/w/namespace.H>
#include <list>

LIBCXXW_NAMESPACE_START

//! A list of all focusable fields.

typedef std::list<focusable_impl> focusable_fields_t;

LIBCXXW_NAMESPACE_END

#endif
