/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef busy_h
#define busy_h

#include "x/w/busy.H"
#include "x/w/generic_windowobj.H"
#include "x/w/elementobj.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN busy_impl : public busy {
 public:

	const ref<generic_windowObj::handlerObj> w;

	busy_impl(elementObj::implObj &i);

	busy_impl(const ref<generic_windowObj::handlerObj> &w);

	~busy_impl();

	x::ref<x::obj> get_mcguffin() const;
};

LIBCXXW_NAMESPACE_END

#endif
