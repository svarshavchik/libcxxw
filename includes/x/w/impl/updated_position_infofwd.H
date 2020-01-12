/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_updated_position_infofwd_h
#define x_w_impl_updated_position_infofwd_h

#include <x/w/namespace.H>
#include <x/w/elementobj.H>
#include <tuple>
#include <vector>
#include <unordered_set>
#include <x/refptr_hash.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN updated_position_info;

struct LIBCXX_HIDDEN updated_position_move_info;

//! Internal container used in widget position update processing.

typedef std::vector<std::tuple<std::unordered_set<element_impl>::iterator,
			       updated_position_move_info>
		    > updated_position_container_t;

LIBCXXW_NAMESPACE_END

#endif
