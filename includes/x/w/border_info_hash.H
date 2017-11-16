/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_border_infohash_H
#define x_w_border_infohash_H

#include <x/w/border_info.H>
#include <x/refptr_hash.H>
#include <x/number_hash.H>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::border_info
	template<>
	struct hash<LIBCXX_NAMESPACE::w::border_info> {

		//! Calculate a hash for a \c INSERT_LIBX_NAMESPACE::w::border_info
		size_t operator()(const LIBCXX_NAMESPACE::w::border_info &b)
			const noexcept
		{
			hash<LIBCXX_NAMESPACE::w::dim_t> dim_t_hash;

			auto h=dim_t_hash(b.width)+
				dim_t_hash(b.height)+
				dim_t_hash(b.radius);

			hash<uint8_t> dash_hash;

			for (const auto dash:b.dashes)
				h += dash_hash(dash);

			hash<LIBCXX_NAMESPACE::w::const_picture> pic_hash;

			for (const auto &color:b.colors)
				h += pic_hash(color);

			return h;
		}
	};
}

#endif
