/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusablefwd_h
#define focusablefwd_h

#include <list>
#include <x/ref.H>
#include "x/w/namespace.H"
#include <list>

LIBCXXW_NAMESPACE_START

class focusableImplObj;

//! An element that has a parent element.

typedef ref<focusableImplObj> focusable_impl;

typedef std::list<focusable_impl> focusable_fields_t;

LIBCXXW_NAMESPACE_END

#endif
