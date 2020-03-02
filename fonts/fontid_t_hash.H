/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontid_T_hash_H
#define fontid_T_hash_H

#include "fonts/fontid_t.H"
#include <x/number_hash.H>
#include <functional>

LIBCXXW_NAMESPACE_START

//! Hash function for a \ref font_id_t

struct LIBCXX_HIDDEN font_id_t_hash :
	public std::hash<std::string>, std::hash<depth_t> {

	using std::hash<std::string>::operator();
	using std::hash<depth_t>::operator();

	size_t operator()(const font_id_t &f) const
	{
		return operator()(f.key)+operator()(f.depth);
	}
};

LIBCXXW_NAMESPACE_END

#endif
