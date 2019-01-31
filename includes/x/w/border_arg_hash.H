/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_border_arg_hash_H
#define x_w_border_arg_hash_H

#include <x/w/border_arg.H>
#include <x/w/border_infomm_hash.H>
#include <x/namespace.h>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::border_arg

	template<>
	struct hash<LIBCXX_NAMESPACE::w::border_arg>
		: public hash<LIBCXX_NAMESPACE::w::border_infomm> {

		size_t operator()(const LIBCXX_NAMESPACE::w::border_arg &a)
			const noexcept;
	};
}
#endif
