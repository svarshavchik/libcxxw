/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgbhash_H
#define x_w_rgbhash_H

#include <x/w/rgb.H>
#include <functional>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::hash
	template<>
	struct hash<LIBCXX_NAMESPACE::w::rgb> {

		inline size_t operator()(const LIBCXX_NAMESPACE::w::rgb &r)
			const noexcept
		{
			return r.value();
		}
	};
}

#endif
