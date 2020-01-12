/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef element_position_updated_set_h
#define element_position_updated_set_h

#include "x/w/namespace.H"
#include "element_position_updated_setfwd.H"
#include "x/w/elementobj.H"
#include <x/refptr_hash.H>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

struct element_position_updated_set_t {

	std::unordered_set<element_impl> elements;
	bool resize_pending;
};

LIBCXXW_NAMESPACE_END

#endif
