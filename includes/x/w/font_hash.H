/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_font_hash_H
#define x_w_font_hash_H

#include <x/w/font.H>
#include <functional>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::font

	template<>
	struct hash<LIBCXX_NAMESPACE::w::font>
		: public hash<string>, hash<int> {

		inline size_t operator()(const LIBCXX_NAMESPACE::w::font &r)
			const noexcept
		{
			return hash<string>::operator()(r.foundry) +
				hash<string>::operator()(r.family) +
				hash<int>::operator()(r.weight) +
				hash<int>::operator()(r.slant) +
				hash<int>::operator()(r.width) +
				hash<string>::operator()(r.style) +
				hash<int>::operator()(r.point_size) +
				hash<int>::operator()(r.spacing);
		}
	};
}

#endif
