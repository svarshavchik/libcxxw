/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_picturehash_H
#define x_w_picturehash_H

#include <x/w/pictureobj.H>

namespace std {

	//! Overload std::hash for a \c LIBCXX_NAMESPACE::w::picture::base::fixedprec

	template<>
	struct hash<LIBCXX_NAMESPACE::w::pictureObj::fixedprec> {

		inline size_t operator()
		(const LIBCXX_NAMESPACE::w::pictureObj::fixedprec &p)
			const noexcept
		{
			return p.value;
		}
	};

	//! Overload std::hash for a \c LIBCXX_NAMESPACE::w::picture::base::point

	template<>
	struct hash<LIBCXX_NAMESPACE::w::pictureObj::point> :
		public hash<LIBCXX_NAMESPACE::w::pictureObj::fixedprec> {

		inline size_t operator()(const LIBCXX_NAMESPACE::w::pictureObj
					 ::point &p)
			const noexcept
		{
			return hash<LIBCXX_NAMESPACE::w::pictureObj::fixedprec>
				::operator()(p.x) +
				hash<LIBCXX_NAMESPACE::w::pictureObj::fixedprec>
				::operator()(p.y);
		}
	};
}

#endif
