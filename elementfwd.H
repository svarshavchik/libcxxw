/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef elementfwd_h
#define elementfwd_h

#include <x/ptrfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/elementobj.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

typedef ref<elementObj::implObj> elementimpl;

typedef ptr<elementObj::implObj> elementimplptr;

// Comparison function implementing strict weak ordering of elements by
// their nesting level. Elements with the same nesting level are ordered
// by their pointers.

class elements_by_nesting_level {

public:

	elements_by_nesting_level()=default;
	~elements_by_nesting_level()=default;

	bool operator()(const elementimpl &a, const elementimpl &b) const;
};

LIBCXXW_NAMESPACE_END

#endif
