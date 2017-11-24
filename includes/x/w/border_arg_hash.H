/*
** Copyright 2017 Double Precision, Inc.
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
	struct hash<LIBCXX_NAMESPACE::w::border_arg> {

		size_t operator()(const LIBCXX_NAMESPACE::w::border_arg &a)
			const
		{
			return visit(LIBCXX_NAMESPACE::visitor{
					[]
					(const LIBCXX_NAMESPACE::w
							::border_infomm &m)
					{
						return hash<LIBCXX_NAMESPACE::w
							    ::border_infomm>
							()(m);
					},
					[]
					(const string &s)
					{
						return hash<string>()(s);
					}}, a);
		}
	};
}
#endif
