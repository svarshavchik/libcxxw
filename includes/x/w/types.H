/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_types_h
#define x_w_types_h

#include <cstdint>

#include <x/number.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

//! Tag used by \ref dim_t

//! Provides the infinite() value for `dim_t`

class dim_tag {

public:

	//! Dimension is infinite/not specified

	static inline constexpr uint16_t infinite()
	{
		return (uint16_t)-1;
	}
};

class dim_squared_tag;
class coord_tag;
class coord_squared_tag;
class depth_tag;

//! The unit type for dimensions: width or height

typedef number<uint16_t, dim_tag, dim_tag> dim_t;

//! The unit type that can accomodate dim_t squared

typedef number<uint32_t, dim_squared_tag> dim_squared_t;

//! The unit type for positions: x and y coordinates

typedef number<int16_t, coord_tag> coord_t;

//! The unit type that can accomodate coord_t + dim_t

typedef number<int32_t, coord_squared_tag> coord_squared_t;

//! The unit type for bit depth of windows, drawables...
typedef number<uint8_t, depth_tag> depth_t;

//! Visual class

enum class visual_class_t {
	static_gray=0,
	gray_scale=1,
	static_color=2,
	pseudo_color=3,
	true_color=4,
	direct_color=5,
};

LIBCXXW_NAMESPACE_END

#endif
