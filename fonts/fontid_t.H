/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontid_t_H
#define fontid_t_H

#include <x/ref.H>
#include <x/weakunordered_multimap.H>
#include <string>
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

//! The key for caching fonts.

//! This is used as a key for two separate sets of font-related objects.
//!
//! - An individual \ref cached_font "cached font".
//!
//! - A \ref fontcollection "collection" of cached fonts.

struct LIBCXX_HIDDEN font_id_t {

	//! Font's key, this comes from unparsing fontpattern.
	std::string key;

	//! Alpha channel depth of the font glyphset.
	depth_t depth;

	//! Constructor
	font_id_t(const std::string &keyArg,
		  depth_t depthArg)
		: key(keyArg), depth(depthArg)
	{
	}

	//! Comparison operator
	bool operator==(const font_id_t &o) const
	{
		return key == o.key && depth == o.depth;
	}

	//! Comparison operator
	bool operator!=(const font_id_t &o) const
	{
		return !operator==(o);
	}
};

struct LIBCXX_HIDDEN font_id_t_hash;

struct font_id_t_hash;

class fontcollectionObj;

//! A container for a cache of instantiated font collections, by \ref font_id_t "identifier".

typedef weakunordered_multimap<font_id_t,
			       fontcollectionObj,
			       font_id_t_hash> sorted_font_cache_t;

LIBCXXW_NAMESPACE_END

#endif
