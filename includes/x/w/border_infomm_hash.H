/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_border_infommhash_H
#define x_w_border_infommhash_H

#include <x/w/border_infomm.H>
#include <x/w/rgb_hash.H>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::border_infomm
	template<>
	struct hash<LIBCXX_NAMESPACE::w::border_infomm>
		: public hash<LIBCXX_NAMESPACE::w::color_arg> {

		//! Calculate a hash for a \c INSERT_LIBX_NAMESPACE::w::border_infomm
		size_t operator()(const LIBCXX_NAMESPACE::w::border_infomm &b)
			const noexcept;
	};
}

#endif
