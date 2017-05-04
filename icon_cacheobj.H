/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_cacheobj_H
#define icon_cacheobj_H

#include "defaultthemefwd.H"
#include "icon_cachefwd.H"
#include "iconobj.H"
#include "sxg/sxg_parserfwd.H"
#include "sxg/sxg_parserobj.H"
#include "sxg/sxg_imagefwd.H"
#include <x/weakunordered_multimapfwd.H>

LIBCXXW_NAMESPACE_START;

//! A cache of icons of various kinds.

//! This cache is attached to each screen object, and contains various icon
//! image-related caches.

//! \see icon_cache

class LIBCXX_HIDDEN icon_cacheObj : virtual public obj {

 public:

	//! Constructor
	icon_cacheObj();

	//! Destructor
	~icon_cacheObj();

	////////////////////////////////////////////////////////////////////
	//
	// SXG icons

	struct sxg_cache_key_t;
	struct sxg_cache_key_t_hash;

	//! Container for the SXG parser cache.

	typedef weakunordered_multimap<sxg_cache_key_t,
				       sxg_parserObj,
				       sxg_cache_key_t_hash>
		sxg_parser_cache_t;

	//! SXG parser cache, used by get_sxg().
	const sxg_parser_cache_t sxg_parser_cache;


	struct sxg_image_cache_key;
	struct sxg_image_cache_key_hash;

	//! Type of the cache of rendered images.

	typedef weakunordered_multimap<sxg_image_cache_key,
				       sxg_imageObj,
				       sxg_image_cache_key_hash>
		sxg_image_cache_t;

	//! Cache of rendered images.
	const sxg_image_cache_t sxg_image_cache;

	////////////////////////////////////////////////////////////////
	//
	// Images whose size is specified in millimeters.

	struct mm_image_cache_key;
	struct mm_image_cache_key_hash;

	//! Type of the cache of rendered images.

	typedef weakunordered_multimap<mm_image_cache_key,
				       iconObj,
				       mm_image_cache_key_hash>
		mm_image_cache_t;

	//! Cache of rendered images.
	const mm_image_cache_t mm_image_cache;

};

LIBCXXW_NAMESPACE_END

#endif
