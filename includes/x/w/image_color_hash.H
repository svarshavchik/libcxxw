/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_image_color_hash_H
#define x_w_image_color_hash_H

#include <x/w/rgbfwd.H>
#include <x/w/dim_arg_hash.H>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::rgb
	template<>
	struct hash<LIBCXX_NAMESPACE::w::image_color>
		: public hash<LIBCXX_NAMESPACE::w::dim_arg> {

		size_t operator()(const LIBCXX_NAMESPACE::w::image_color &ic)
			const noexcept;
	};

}

#endif
