/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_picture_h
#define x_w_picture_h

#include <x/w/picturefwd.H>
#include <x/w/pictureobj.H>
#include <x/ref.H>

#include <string_view>

LIBCXXW_NAMESPACE_START

//! Base class of \ref picture "pictures".

class pictureBase : public ptrref_base {

public:

	//! typedef alias

	typedef pictureObj::clip_mask clip_mask;

	//! typedef alias

	typedef pictureObj::rectangular_clip_mask rectangular_clip_mask;

	//! typedef alias

	typedef pictureObj::fixedprec fixedprec;

	//! typedef alias

	typedef pictureObj::point point;

	//! typedef alias

	typedef pictureObj::triangle triangle;

	//! Convert operation value to string

	static std::string render_pict_op_to_string(render_pict_op);

	//! Convert operation value from string

	static render_pict_op render_pict_op_from_string(const std::string_view &);

	//! Convert repeat value to string

	static std::string render_repeat_tostring(render_repeat);

	//! Convert repeat value from string

	static render_repeat render_repeat_from_string(const std::string_view &);

};

//! Default constructor
inline constexpr pictureObj::fixedprec::fixedprec() noexcept : value{0} {}

//! Constructor
inline constexpr pictureObj::fixedprec::fixedprec(int16_t integer,
						  uint16_t fraction) noexcept
: value{(static_cast<value_t>(integer) << 16) | fraction}
{
}

//! Constructor
inline constexpr pictureObj::fixedprec::fixedprec(const coord_t &c) noexcept
: fixedprec{xcoord_t::truncate(c)}
{
}

//! Helper for constructing a fixedprec from a double

struct pictureObj::fixedprec::double_constructor_helper {

	double integral;

	double fraction;

	explicit constexpr
	double_constructor_helper(double v)
	noexcept : integral{0}, fraction(std::abs(std::modf(v, &integral)))
	{
	}
};

//! Helper for constructing a fixedprec from an int16_t

struct pictureObj::fixedprec::intprec_constructor_helper {

	int16_t value;

	explicit constexpr intprec_constructor_helper(int16_t v) : value(v)
	{
	}
};

//! Helper for constructing fixedprec from a single int16_t value.

template<typename T>
struct pictureObj::fixedprec::constructor_helper<T, true, false>
	: intprec_constructor_helper
{
	using intprec_constructor_helper::intprec_constructor_helper;
};

//! Helper for constructing fixedprec from a single double value.

template<typename T>
struct pictureObj::fixedprec::constructor_helper<T, false, true>
	: double_constructor_helper
{
	using double_constructor_helper::double_constructor_helper;
};

//! Single argument constructor

//! Accepts either an int16_t or a double value

template<typename T, typename>
inline constexpr pictureObj::fixedprec::fixedprec(T int16_or_double_value) noexcept
: fixedprec{constructor_helper<T>{int16_or_double_value}}
{
}

//! Constructor

//! Accept only an int16_t parameter, or smaller.

inline constexpr pictureObj::fixedprec
::fixedprec(const intprec_constructor_helper &v) noexcept
: fixedprec{v.value, 0}
{
}

//! Constructor
inline constexpr pictureObj::fixedprec
::fixedprec(const double_constructor_helper &v) noexcept
: fixedprec{xcoord_t::truncate(std::trunc(v.integral)),
		xdim_t::truncate(std::trunc
				 (v.fraction
				  * (xdim_t::value_type)(-1)))
		}
{
}

//! Comparison operator
inline constexpr int pictureObj::fixedprec::compare(const fixedprec &o) const
{
	return value < o.value ? -1:value > o.value ? 1: 0;
}

//! Comparison operator

inline constexpr bool pictureObj::fixedprec::operator<(const fixedprec &o) const
{
	return compare(o) < 0;
}

//! Comparison operator

inline constexpr bool pictureObj::fixedprec::operator<=(const fixedprec &o)
	const
{
	return compare(o) <= 0;
}

//! Comparison operator

inline constexpr bool pictureObj::fixedprec::operator>(const fixedprec &o) const
{
	return compare(o) > 0;
}

//! Comparison operator

inline constexpr bool pictureObj::fixedprec::operator>=(const fixedprec &o)
	const
{
	return compare(o) >= 0;
}

//! Comparison operator
inline constexpr bool pictureObj::fixedprec::operator==(const fixedprec &o)
	const
{
	return compare(o) == 0;
}

//! Comparison operator
inline constexpr bool pictureObj::fixedprec::operator!=(const fixedprec &o)
	const
{
	return compare(o) != 0;
}

//! Distance between two points.

inline constexpr uint32_t pictureObj::fixedprec::distance(const fixedprec &o)
	const
{
	return value < o.value ?
		       int64_t{o.value}-value : int64_t{value}-o.value;
}

//! Borrow number.H to safely truncate a value

template<typename T>
auto inline constexpr pictureObj::fixedprec::truncate(T &&v)
{
	return number<T, void>::truncate(std::forward<T>(v));
}

//! Comparison operator

inline constexpr int pictureObj::point::compare(const point &o) const noexcept
{
	int v=x.compare(o.x);

	if (!v)
		v=y.compare(o.y);

	return v;
}

//! Comparison operator

constexpr inline bool pictureObj::point::operator<(const point &o)
	const noexcept
{
	return compare(o) < 0;
}

//! Comparison operator

constexpr inline bool pictureObj::point::operator==(const point &o)
	const noexcept
{
	return compare(o) == 0;
}

//! Comparison operator

inline constexpr int pictureObj::triangle::compare(const triangle &o) const
{
	int v=p1.compare(o.p1);

	if (!v) v=p2.compare(o.p2);

	if (!v) v=p3.compare(o.p3);

	return v;
}

//! Comparison operator

inline constexpr bool pictureObj::triangle::operator<(const triangle &o) const
{
	return compare(o) < 0;
}

//! An internal implementation object of a \ref picture "picture".

typedef ref<pictureObj::implObj> picture_internal;

//! A constant internal implementation object of a \ref picture "picture".

typedef ref<pictureObj::implObj> const_picture_internal;

LIBCXXW_NAMESPACE_END

#endif
