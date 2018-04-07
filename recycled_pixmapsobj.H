/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef recycled_pixmapsobj_h
#define recycled_pixmapsobj_h

#include "recycled_pixmapsfwd.H"
#include "x/w/impl/background_colorfwd.H"
#include "x/w/pictformatfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/rgb.H"
#include "x/w/types.H"
#include <x/obj.H>
#include <x/refptr_hash.H>
#include <x/weakunordered_multimapfwd.H>
#include <string>

LIBCXXW_NAMESPACE_START

//! A \ref recycled_pixmaps "recycled pixmap and picture container".

class LIBCXX_HIDDEN recycled_pixmapsObj : virtual public obj {

	//! Key used by scratch buffers.

	//! The key consists of an identifier and a pictformat.

	struct scratch_buffer_key {

		//! A label for a particular scratch buffer.
		std::string identifier;

		//! Scratch buffer's pictformat.
		const_pictformat pf;

		//! Equality comparison
		bool operator==(const scratch_buffer_key &) const noexcept;
	};


	//! A hasher for a \c scratch_buffer_key

	struct scratch_buffer_key_hash {

		//! Hash a \c scratch_buffer_key

		size_t operator()(const scratch_buffer_key &) const noexcept;
	};

public:
	//! Container for the scratch buffer cache.

	typedef weakunordered_multimap<scratch_buffer_key,
				       scratch_bufferObj,
				       scratch_buffer_key_hash>
		scratch_buffer_cache_t;

	//! The cache of scratch buffers.

	scratch_buffer_cache_t scratch_buffer_cache;

	//! Container for a cache of theme background colors

	typedef weakunordered_multimap<color_arg, background_colorObj>
		theme_background_color_cache_t;

	theme_background_color_cache_t theme_background_color_cache;

	//! Container for a cache of non-theme background colors

	typedef weakunordered_multimap<const_picture, background_colorObj>
		nontheme_background_color_cache_t;

	nontheme_background_color_cache_t nontheme_background_color_cache;

	//! Key used for the gradient cache.

	struct gradient_key {

		background_color base_background;
		const_picture gradient_picture;

		bool operator==(const gradient_key &)
			const noexcept;
	};

	//! A hashed for a \c gradient_key

	struct gradient_key_hash
		: public std::hash<background_color>,
		std::hash<const_picture> {

		//! Hash a \c gradient_key

		size_t operator()(const gradient_key &) const noexcept;
	};

	//! Container for the scratch buffer cache.

	typedef weakunordered_multimap<gradient_key,
				       background_colorObj,
				       gradient_key_hash>
		gradient_cache_t;

	//! The cache of scratch buffers.

	gradient_cache_t gradient_cache;

	//! Constructor

	recycled_pixmapsObj();

	//! Destructor

	~recycled_pixmapsObj();
};

LIBCXXW_NAMESPACE_END

#endif
