/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_rgbhash_H
#define x_w_rgbhash_H

#include <x/w/rgb.H>
#include <x/w/image_color_hash.H>
#include <functional>
#include <variant>

namespace std {

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::rgb
	template<>
	struct hash<LIBCXX_NAMESPACE::w::rgb> {

		inline size_t operator()(const LIBCXX_NAMESPACE::w::rgb &r)
			const noexcept
		{
			return r.value();
		}
	};


	//! \c std::hash can handle an rgb_gradient just fine.

	template<>
	struct hash<LIBCXX_NAMESPACE::w::rgb_gradient> {
		size_t operator()(const LIBCXX_NAMESPACE::w::rgb_gradient &r)
			const noexcept;
	};

	//! Specialization of \c std::hash for \c INSERT_LIBX_NAMESPACE::w::color_arg
	template<>
	struct hash<LIBCXX_NAMESPACE::w::color_arg> {

		size_t operator()(const LIBCXX_NAMESPACE::w::color_arg &r)
			const noexcept;
	};
}

#endif
