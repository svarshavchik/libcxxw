/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_picturecache_h
#define screen_picturecache_h

#include "screen_picturecachefwd.H"
#include "x/w/rgb.H"
#include "x/w/rgb_hash.H"
#include "x/w/picturefwd.H"
#include "x/w/types.H"
#include <x/weakunordered_multimapfwd.H>
#include <x/obj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! A cache of pictures for this screen.

class LIBCXX_HIDDEN screen_picturecacheObj : virtual public obj {

 public:
	//! A container for solid color pictures

	typedef x::weakunordered_multimap<rgb, pictureObj> solid_color_map_t;

	//! Solid color picture cache
	const solid_color_map_t solid_colors;

	//! Caching key for gradient colors

	struct linear_gradient_cache_key_t {
		rgb_gradient g;
		coord_t x1;
		coord_t y1;
		coord_t x2;
		coord_t y2;
		render_repeat repeat;

		bool operator==(const linear_gradient_cache_key_t &)
			const noexcept;
	};

	//! Compute a hash key

	struct linear_gradient_cache_key_t_hash
		: public std::hash<rgb_gradient> {
		size_t operator()(const linear_gradient_cache_key_t &)
			const noexcept;
	};

	//! A container for linear gradients.

	typedef x::weakunordered_multimap<linear_gradient_cache_key_t,
					  pictureObj,
					  linear_gradient_cache_key_t_hash
					  > linear_gradient_map_t;

	//! Linear gradients

	const linear_gradient_map_t linear_gradients;

	//! Constructor
	screen_picturecacheObj();

	//! Destructor
	~screen_picturecacheObj();
};

LIBCXXW_NAMESPACE_END

#endif
