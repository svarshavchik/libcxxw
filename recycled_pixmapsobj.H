/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef recycled_pixmapsobj_h
#define recycled_pixmapsobj_h

#include "recycled_pixmapsfwd.H"
#include "x/w/impl/background_colorfwd.H"
#include "x/w/impl/iconfwd.H"
#include "x/w/pictformatfwd.H"
#include "x/w/scratch_bufferfwd.H"
#include "x/w/picturefwd.H"
#include "x/w/image_color_hash.H"
#include "x/w/rgb.H"
#include "x/w/types.H"
#include <x/obj.H>
#include <x/refptr_hash.H>
#include <x/number_hash.H>
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

	const theme_background_color_cache_t theme_background_color_cache;

	//! Container for a cache of non-theme background colors

	typedef weakunordered_multimap<const_picture, background_colorObj>
		nontheme_background_color_cache_t;

	const nontheme_background_color_cache_t nontheme_background_color_cache;

	//! Key used for the gradient cache.

	struct gradient_key {

		background_color base_background;
		const_picture gradient_picture;

		bool operator==(const gradient_key &)
			const noexcept;
	};

	//! A hash for a \c gradient_key

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

	const gradient_cache_t gradient_cache;

	//! Key used for the image background color cache

	struct image_color_key {

		image_color key_image_color;
		const_pictformat key_pictformat;

		bool operator==(const image_color_key &o) const noexcept
		{
			return key_image_color == o.key_image_color &&
				key_pictformat == o.key_pictformat;
		}
	};

	//! A hash for a \c image_color_key

	struct image_color_key_hash
		: std::hash<image_color>,
		std::hash<const_pictformat> {

		//! Hash a \c image_color_key

		size_t operator()(const image_color_key &) const noexcept;
	};

	//! Container for a cache of icon-based background colors
	typedef weakunordered_multimap<image_color_key,
				       background_colorObj,
				       image_color_key_hash>
		image_background_color_cache_t;

	//! A cache of resizable icon-based background colors
	const image_background_color_cache_t
		image_background_color_cache;

	//! Key used for the image background color cache

	struct element_specific_image_key {

		background_color key_background_color;
		dim_t width;
		dim_t height;

		bool operator==(const element_specific_image_key &o)
			const noexcept
		{
			return key_background_color==o.key_background_color &&
				width == o.width && height == o.height;
		}
	};

	//! A hash for a \c element_specific_image_key

	struct element_specific_image_key_hash
		: std::hash<background_color>,
		std::hash<dim_t> {

		//! Hash a \c element_specific_image_key

		size_t operator()(const element_specific_image_key &)
			const noexcept;
	};

	//! Container for a cache of icon-based background colors
	typedef weakunordered_multimap<element_specific_image_key,
				       background_colorObj,
				       element_specific_image_key_hash>
		element_specific_image_background_color_cache_t;

	//! A cache of resizable icon-based background colors
	const element_specific_image_background_color_cache_t
		element_specific_image_background_color_cache;

	//! Constructor

	recycled_pixmapsObj();

	//! Destructor

	~recycled_pixmapsObj();
};

LIBCXXW_NAMESPACE_END

#endif
