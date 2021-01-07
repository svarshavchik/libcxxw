/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_dimarg_h
#define x_w_dimarg_h

#include <x/w/namespace.H>
#include <string>
#include <variant>
#include <limits>

LIBCXXW_NAMESPACE_START

//! Helper class for constructing a \ref dim_arg "dim_arg" from a char string

//! Inherits from std::string constructor but explicitly deletes a constructor
//! that takes an int as a parameter. This used with SFINAE to eliminate
//! the std::string version of dim_arg from getting constructed from a literal
//! "0", erroneously matching std::string's "const char *" constructor.

struct dim_arg_string_init : public std::string {

	using std::string::string;

	dim_arg_string_init(int)=delete;
	dim_arg_string_init(const std::string &);
	dim_arg_string_init(std::string &&);
};

//! Determine the appropriate variant index of \ref dim_arg "dim_arg"'s constructor

//! Determines which variant alternative should be constructed by the
//! given parameter.

template<typename T, typename=void> struct is_numeric_or_string;

//! Specialization for constructing a \ref dim_arg "dim_arg" for a string name.

//! Uses SFINAE to resolve only for a parameter to \ref dim_arg "dim_arg"
//! constructor that's a character string.

template<typename T>
struct is_numeric_or_string<
	T,
	std::void_t<decltype(dim_arg_string_init{std::declval<T &&>()})>>
{
	static constexpr std::size_t index=0;
};

//! Helper for avoiding ill-formed usage of numeric_limits with arrays.

//! Instantiating std::numeric_limits for an array type is ill-formed,
//! use SFINAE specialization to avoid this.

template<typename T> struct is_numeric_type : std::numeric_limits<T> {};

//! Array types are not basic numeric values

//! Specialization that avoids instantiating std::numeric_limits for array
//! types. This used by \ref is_numeric_or_string "is_numeric_or_string"
//! SFINAE type to select a numeric or a string variant alternative fo
//! \ref dim_arg "dim_arg" variant.

template<typename T, std::size_t n>
struct is_numeric_type<T [n]> {};

//! Specialization for constructing a \ref dim_arg "dim_arg" for a numeric value

//! Uses SFINAE to resolve only for a parameter to \ref dim_arg "dim_arg"
//! constructor that's a numeric value.

template<typename T>
struct is_numeric_or_string<T,
			    std::enable_if_t
			    <is_numeric_type<T>::is_specialized>>
{
	static constexpr std::size_t index=1;
};

//! Helper alias for \ref dim_arg "dim_arg"'s constructor.

template<typename T>
using is_numeric_or_string_v=typename is_numeric_or_string
	<std::remove_cv_t<std::remove_reference_t<T>>>::type;

//! A dimension specification.

//! Certain parameters may be specified either as a numeric value, or as
//! a text string. A numerical value specifies a size expressed in
//! millimeters. A text string references a size specified by the current
//! display theme (this is mostly for the library's internal use).
//!
//! The numeric value must be positive. The actual number of pixels gets
//! computed based on the current theme's scaling factor. A non-0
//! dimension is always at least one pixel.
//!
//! This is a tiny subclass of std::variant, providing a helping hand
//! in constructing it.

struct dim_arg : std::variant<std::string, double> {

	//! Alias.
	typedef std::variant<std::string, double> variant_t;

	//! Constructor

	//! Determine the appropriate variant_t constructor based on the
	//! parameter, to avoid some ambigous edge cases.

	template<typename T,
		 std::size_t n=
		 is_numeric_or_string
		 <std::remove_cv_t<std::remove_reference_t<T>>>::index>
	dim_arg(T &&t)
		: variant_t{std::in_place_index_t<n>{}, std::forward<T>(t)}
	{
	}

	//! Default constructor
	dim_arg() : variant_t{std::in_place_index_t<0>{}}
	{
	}

	//! Assignment operator

	//! Determine the appropriate variant_t alternative to assign
	//! based on the parameter, to avoid some ambigous edge cases.
	template<typename T,
		 std::size_t n=
		 is_numeric_or_string
		 <std::remove_cv_t<std::remove_reference_t<T>>>::index>
	dim_arg &operator=(T &&t)
	{
		variant_t::emplace<n>(std::forward<T>(t));
		return *this;
	}

	//! Helper to return the underlying variant type
	variant_t &variant() { return *this; }

	//! Helper to return the underlying variant type
	const variant_t &variant() const { return *this; }
};

LIBCXXW_NAMESPACE_END

#endif
