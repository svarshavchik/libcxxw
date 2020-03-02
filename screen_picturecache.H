/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_picturecache_h
#define screen_picturecache_h

#include "screen_picturecachefwd.H"
#include "x/w/rgb.H"
#include "x/w/rgb_hash.H"
#include "x/w/picture.H"
#include "x/w/picture_hash.H"
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
		picture::base::fixedprec x1;
		picture::base::fixedprec y1;
		picture::base::fixedprec x2;
		picture::base::fixedprec y2;
		render_repeat repeat;

		bool operator==(const linear_gradient_cache_key_t &)
			const noexcept;
	};

	//! Compute a hash key

	struct linear_gradient_cache_key_t_hash
		: public std::hash<rgb_gradient>,
		  public std::hash<picture::base::fixedprec> {
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

	//! Caching key for gradient colors

	struct radial_gradient_cache_key_t {
		rgb_gradient g;
		picture::base::fixedprec inner_x;
		picture::base::fixedprec inner_y;
		picture::base::fixedprec outer_x;
		picture::base::fixedprec outer_y;
		picture::base::fixedprec inner_radius;
		picture::base::fixedprec outer_radius;

		render_repeat repeat;

		bool operator==(const radial_gradient_cache_key_t &)
			const noexcept;
	};

	//! Compute a hash key

	struct radial_gradient_cache_key_t_hash
		: public std::hash<rgb_gradient>,
		  public std::hash<picture::base::fixedprec> {
		size_t operator()(const radial_gradient_cache_key_t &)
			const noexcept;
	};

	//! A container for linear gradients.

	typedef x::weakunordered_multimap<radial_gradient_cache_key_t,
					  pictureObj,
					  radial_gradient_cache_key_t_hash
					  > radial_gradient_map_t;

	//! Linear gradients

	const radial_gradient_map_t radial_gradients;

	//! Constructor
	screen_picturecacheObj();

	//! Destructor
	~screen_picturecacheObj();
};

LIBCXXW_NAMESPACE_END

#endif
