/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_types_h
#define x_w_types_h

#include <cstdint>

#include <x/number.H>
#include <x/w/namespace.H>

LIBCXXW_NAMESPACE_START

#include <x/w/constants_inc.H>

template<typename number_1, typename number_2> class valid_add_op;
template<typename number_1, typename number_2> class valid_sub_op;
template<typename number_1, typename number_2> class valid_mul_op;
template<typename number_1, typename number_2> class valid_divmod_op;

//! Base class for custom types.

//! Used by \c x::number to define the valid operation on custom types, and
//! the resulting types.

class types_base {

public:

	template<typename number_1, typename number_2>
	using resulting_add_op=typename valid_add_op<number_1, number_2>::type;

	//! Define resulting type for subtraction operations.
	template<typename number_1, typename number_2>
	using resulting_sub_op=typename valid_sub_op<number_1, number_2>::type;

	//! Define resulting type for multiplication operations.
	template<typename number_1, typename number_2>
	using resulting_mul_op=typename valid_mul_op<number_1, number_2>::type;

	//! Define resulting type for division operations.
	template<typename number_1, typename number_2>
	using resulting_div_op=typename valid_divmod_op<number_1, number_2>::type;

	//! Define resulting type for modulus operations.
	template<typename number_1, typename number_2>
	using resulting_mod_op=typename valid_divmod_op<number_1, number_2>::type;

	//! Default unary negation operation.
	template<typename number_1>
	using resulting_uneg_op=number_default_uneg_op<number_1>;
};

//! Tag used by \ref dim_t

//! Provides the infinite() value for `dim_t`

class dim_base : public types_base {

public:

	//! Dimension is infinite/not specified

	static inline constexpr uint32_t infinite()
	{
		return (uint32_t)-1;
	}
};

class xdim_tag;
class dim_tag;
class dim_squared_tag;
class xcoord_tag;
class coord_tag;
class coord_squared_tag;
class depth_tag;

//! A dimension (size) in the X protocol is a 16 bit value.
typedef number<uint16_t, xdim_tag> xdim_t;

//! An X protocol coordinate is a 16 bit value.

typedef number<int16_t, xcoord_tag> xcoord_t;

//! Internal unit type for dimensions: width or height

typedef number<uint32_t, dim_tag, dim_base> dim_t;

//! The unit type that can accomodate dim_t squared

typedef number<uint64_t, dim_squared_tag, types_base> dim_squared_t;

//! Internal unit type for positions: x and y coordinates

typedef number<int32_t, coord_tag, types_base> coord_t;

//! The unit type that can accomodate coord_t + dim_t

typedef number<int64_t, coord_squared_tag, types_base> coord_squared_t;

//! The unit type for bit depth of windows, drawables...
typedef number<uint8_t, depth_tag> depth_t;

//! Adding a coordinate to a coordinate

template<>
class valid_add_op<coord_t, coord_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<coord_t, dim_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<dim_t, coord_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<coord_t, dim_squared_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<dim_squared_t, coord_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<coord_squared_t, dim_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<dim_t, coord_squared_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<coord_squared_t, dim_squared_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding a dimension to a coordinate.

template<>
class valid_add_op<dim_squared_t, coord_squared_t> {

public:

	//! The result is coord_squared_t

	typedef coord_squared_t type;
};

//! Adding dimensions together

template<>
class valid_add_op<dim_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Adding dimensions together

template<>
class valid_add_op<dim_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Adding dimensions together

template<>
class valid_add_op<dim_squared_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Adding dimensions together

template<>
class valid_add_op<dim_squared_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Coordinates can be subtracted from each one.

template<>
class valid_sub_op<coord_t, coord_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! A dimension can be subtracted from a coordinate.

template<>
class valid_sub_op<coord_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef coord_squared_t type;
};

template<>
class valid_sub_op<coord_squared_t, coord_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be subtracted from each other.

template<>
class valid_sub_op<dim_t, dim_t> {

public:
	//! The result is dim_t

	typedef dim_t type;
};

//! Dimensions can be subtracted from each other.

template<>
class valid_sub_op<dim_t, dim_squared_t> {

public:
	//! The result is dim_t

	typedef dim_t type;
};

//! Dimensions can be subtracted from each other.

template<>
class valid_sub_op<dim_squared_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be subtracted from each other.

template<>
class valid_sub_op<dim_squared_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be multiplied with each other.

template<>
class valid_mul_op<dim_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be multiplied with each other.

template<>
class valid_mul_op<dim_squared_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be multiplied with each other.

template<>
class valid_mul_op<dim_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be multiplied with each other.

template<>
class valid_mul_op<dim_squared_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};



//! Dimensions can be divided by each other.

template<>
class valid_divmod_op<dim_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_t type;
};

//! Dimensions can be divided by each other.

template<>
class valid_divmod_op<dim_squared_t, dim_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

//! Dimensions can be divided by each other.

template<>
class valid_divmod_op<dim_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_t type;
};

//! Dimensions can be divided by each other.

template<>
class valid_divmod_op<dim_squared_t, dim_squared_t> {

public:
	//! The result is dim_squared_t

	typedef dim_squared_t type;
};

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
