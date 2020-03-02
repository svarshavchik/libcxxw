/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_dim_arg_hash_H
#define x_w_dim_arg_hash_H

#include <x/w/dim_arg.H>
#include <utility>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::dim_arg
	template<>
	struct hash<LIBCXX_NAMESPACE::w::dim_arg> : hash<string> {

		//! Calculate a hash for a \c INSERT_LIBX_NAMESPACE::w::border_infomm
		size_t operator()(const LIBCXX_NAMESPACE::w::dim_arg &d)
			const noexcept;
	};
}

#endif
