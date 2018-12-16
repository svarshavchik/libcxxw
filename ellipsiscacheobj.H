/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_ellipsiscacheobj_h
#define x_w_ellipsiscacheobj_h

#include "ellipsiscachefwd.H"
#include "x/w/impl/richtext/richtextfwd.H"
#include "x/w/impl/fonts/current_fontcollectionfwd.H"
#include "x/w/rgb.H"
#include "x/w/rgb_hash.H"
#include "x/w/elementobj.H"
#include <x/refptr_hash.H>
#include <x/weakunordered_mapfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see ellipsiscache

class LIBCXX_HIDDEN ellipsiscacheObj : virtual public obj {

public:

	//! Ellipsis text string are cached for their given font and color.

	struct ellipsis_cache_key_t {
		current_fontcollection ellipsis_font;
		color_arg ellipsis_color;

		//! Comparison operator
		bool operator==(const ellipsis_cache_key_t &) const;
	};

	//! Hash function for the ellipsis_cache_key_t

	struct ellipsis_cache_key_hash
		: public std::hash<current_fontcollection>,
		  public std::hash<color_arg> {

		// Hash function.

		size_t operator()(const ellipsis_cache_key_t &) const;
	};

	typedef weakunordered_map<ellipsis_cache_key_t,
				  richtextObj,
				  ellipsis_cache_key_hash> cache_t;

	//! The cache
	const cache_t cache;

	//! Constructor
	ellipsiscacheObj();

	//! Destructor
	~ellipsiscacheObj();

	//! Get the ellipsis in the given font.

	richtext get(elementObj::implObj &parent_element);
};

LIBCXXW_NAMESPACE_END

#endif
