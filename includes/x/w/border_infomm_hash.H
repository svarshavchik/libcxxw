/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_border_infommhash_H
#define x_w_border_infommhash_H

#include <x/w/border_infomm.H>
#include <x/refptr_hash.H>

namespace std {

	//! Specialization of \c std::hash for \c LIBCXX_NAMESPACE::w::border_infomm
	template<>
	struct hash<LIBCXX_NAMESPACE::w::border_infomm> {

		//! Calculate a hash for a \c LIBCXX_NAMESPACE::w::border_infomm
		size_t operator()(const LIBCXX_NAMESPACE::w::border_infomm &b)
			const noexcept
		{
			auto s=b.width * 10000 + b.height * 100 +
				b.radius * 10 + (b.rounded ? 1:0);

			for (const auto dash:b.dashes)
				s += dash;

			size_t h=s;

			hash<LIBCXX_NAMESPACE::w::const_picture> pic_hash;

			for (const auto &color:b.colors)
				h += pic_hash(color);

			return h;
		}
	};
}

#endif