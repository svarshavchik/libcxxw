/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_cacheobj_H
#define icon_cacheobj_H

#include "x/w/defaultthemefwd.H"
#include "icon_cachefwd.H"
#include "x/w/impl/pixmap_with_picturefwd.H"
#include "x/w/impl/iconobj.H"
#include "sxg/sxg_parserfwd.H"
#include "sxg/sxg_parserobj.H"
#include "x/w/pixmapfwd.H"
#include "x/w/pixmapobj.H"
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
	// Cache of gif, jpeg, and png files that are loaded into x
	// pixmapObj::implObj.

	struct std_fmt_pixmap_cache_key_t;
	struct std_fmt_pixmap_cache_key_t_hash;

	//! Container for the pixmap cache.
	typedef weakunordered_multimap<std_fmt_pixmap_cache_key_t,
				       pixmapObj::implObj,
				       std_fmt_pixmap_cache_key_t_hash
				       > std_fmt_pixmap_cache_t;

	//! The pixmap cache.

	const std_fmt_pixmap_cache_t std_fmt_pixmap_cache;

	////////////////////////////////////////////////////////////////////
	//
	// SXG icons

	struct sxg_parser_cache_key_t;
	struct sxg_parser_cache_key_t_hash;

	//! Container for the SXG parser cache.

	typedef weakunordered_multimap<sxg_parser_cache_key_t,
				       sxg_parserObj,
				       sxg_parser_cache_key_t_hash>
		sxg_parser_cache_t;

	const sxg_parser_cache_t sxg_parser_cache;

	//////////////////////////////////////////////////////////////////
	//
	// Cache of SXG files scaled to a particular size, and rendered
	// into a pixmapObj::implObj.

	struct sxg_pixmap_cache_key;
	struct sxg_pixmap_cache_key_hash;

	//! Type of the cache of rendered images.

	typedef weakunordered_multimap<sxg_pixmap_cache_key,
				       pixmapObj::implObj,
				       sxg_pixmap_cache_key_hash>
		sxg_pixmap_cache_t;

	//! Cache of rendered images.
	const sxg_pixmap_cache_t sxg_pixmap_cache;

	///////////////////////////////////////////////////////////////////
	//
	// Cache of picture_with_pixmaps for all loaded image types

	struct pixmap_with_picture_cache_key;
	struct pixmap_with_picture_cache_key_hash;

	//! Tpe of the cache for loaded pixmap_with_pictures.

	typedef weakunordered_multimap<pixmap_with_picture_cache_key,
				       pixmap_with_pictureObj,
				       pixmap_with_picture_cache_key_hash>
		pixmap_with_picture_cache_t;

	//! Cache of loaded pixmap_with_pictures.

	const pixmap_with_picture_cache_t pixmap_with_picture_cache;

	// Cache of icons created from pixmap_with_pictures

	struct pixmap_icon_cache_key_t;
	struct pixmap_icon_cache_key_t_hash;

	//! Container for the pixmap icon cache.
	typedef weakunordered_multimap<pixmap_icon_cache_key_t,
				       iconObj,
				       pixmap_icon_cache_key_t_hash
				       > pixmap_icon_cache_t;

	//! The pixmap_icon cache.

	const pixmap_icon_cache_t pixmap_icon_cache;

};

LIBCXXW_NAMESPACE_END

#endif
