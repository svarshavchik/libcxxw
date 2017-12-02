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
#include "x/w/pixmapfwd.H"
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
	// Cache the results of searching for filenames.

	struct extension_cache_key_t;
	struct extension_cache_key_t_hash;
	class cached_filename_infoObj;

	//! Container for a cache of searched icon names, and filenames.

	typedef weakunordered_multimap<extension_cache_key_t,
				       cached_filename_infoObj,
				       extension_cache_key_t_hash
				       > extension_cache_t;

	//! Cached of searchied icon names, and the located filenames.

	const extension_cache_t extension_cache;

	////////////////////////////////////////////////////////////////////
	//
	// Cache of loaded gif, jpeg, png image files.
	struct pixmap_cache_key_t;
	struct pixmap_cache_key_t_hash;

	class pixmap_and_pictureObj;

	//! Container for the pixmap cache.
	typedef weakunordered_multimap<pixmap_cache_key_t,
				       pixmap_and_pictureObj,
				       pixmap_cache_key_t_hash> pixmap_cache_t;

	//! The pixmap cache.

	const pixmap_cache_t pixmap_cache;

	// Cache of icons for loaded gif, jpeg, png image pixmaps.

	struct pixmap_icon_cache_key_t;
	struct pixmap_icon_cache_key_t_hash;

	//! Container for the pixmap icon cache.
	typedef weakunordered_multimap<pixmap_icon_cache_key_t,
				       iconObj,
				       pixmap_icon_cache_key_t_hash
				       > pixmap_icon_cache_t;

	//! The pixmap_icon cache.

	const pixmap_icon_cache_t pixmap_icon_cache;

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
				       icon_imageObj,
				       sxg_image_cache_key_hash>
		sxg_image_cache_t;

	//! Cache of rendered images.
	const sxg_image_cache_t sxg_image_cache;

	////////////////////////////////////////////////////////////////
	//
	// SXG icon cache.

	struct sxg_icon_cache_key;
	struct sxg_icon_cache_key_hash;

	//! Type of the cache of SXG icons.

	typedef weakunordered_multimap<sxg_icon_cache_key,
				       iconObj,
				       sxg_icon_cache_key_hash>
		sxg_icon_cache_t;

	//! Cache of rendered SXG icons.
	const sxg_icon_cache_t sxg_icon_cache;

};

LIBCXXW_NAMESPACE_END

#endif
