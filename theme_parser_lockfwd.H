/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef theme_parser_lockfwd_H
#define theme_parser_lockfwd_H

#include "x/w/namespace.H"
#include "x/w/types.H"
#include "x/w/alignment.H"
#include "x/w/scrollbarfwd.H"
#include "x/w/uigeneratorsfwd.H"
#include "x/w/border_infommfwd.H"
#include "x/w/screenobj.H"
#include "x/w/impl/border_infofwd.H"
#include "x/w/labelfwd.H"
#include "x/w/input_field_config.H"
#include <unordered_set>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

namespace ui {
	class parser_lock;
}

//! Read a single value.

//! When parsing XML-specified layouts,
//! this reads a required xpath.
std::string single_value(const ui::parser_lock &lock,
			 const char *element,
			 const char *parent) LIBCXX_HIDDEN;

//! Read multiple values

//! When parsing XML-specified layouts,
//! this reads collects zero or more elements into a vector.
std::vector<std::string> multiple_values(const ui::parser_lock &lock,
					 const char *element,
					 const char *parent) LIBCXX_HIDDEN;

//! Read an option value.

//! When parsing XML-specified layouts,
//! this reads an optional xpath if it exists, else returns an empty string.
std::string optional_value(const ui::parser_lock &lock,
			   const char *element,
			   const char *parent) LIBCXX_HIDDEN;


//! Read a single value, convert it to lowercase.

//! When parsing XML-specified layouts,
//! this reads a single_value() and converts it to lowercase.
std::string lowercase_single_value(const ui::parser_lock &lock,
				   const char *element,
				   const char *xpath) LIBCXX_HIDDEN;

//! Read a single value.

//! When parsing XML-specified layouts,
//! this checks whether the given xpath exists.
bool single_value_exists(const ui::parser_lock &lock,
			 const char *element) LIBCXX_HIDDEN;

//! Read a single value.

//! \overload
inline bool single_value_exists(const ui::parser_lock &lock,
				const char *element,
				const char *parent)
{
	return single_value_exists(lock, element);
}

template<auto v>
constexpr auto optional_constant(const ui::parser_lock &lock,
				 const char *element,
				 const char *parent)
	LIBCXX_HIDDEN;

//! If the given xpath exists, return a constant.

template<auto v> constexpr auto optional_constant(const ui::parser_lock &lock,
						  const char *element,
						  const char *parent)
{
	return v;
}


//! Read a value

//! When parsing XML-specified layouts,
//! this reads a scrollbar_visibility value.

scrollbar_visibility to_scrollbar_visibility(const ui::parser_lock &,
					     const char *element,
					     const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a dim_t value.
dim_t to_dim_t(const ui::parser_lock &lock,
	       const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a size_t value.
size_t to_size_t(const ui::parser_lock &lock,
		 const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a number between 0-100.
int to_percentage_t(const ui::parser_lock &lock,
		    const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads an halign value.
halign to_halign(const ui::parser_lock &lock,
		 const char *element, const char *parent) LIBCXX_HIDDEN;


//! Read a value

//! When parsing XML-specified layouts,
//! this reads a bidi value.

bidi to_bidi_direction(const ui::parser_lock &lock,
		       const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value

//! When parsing XML-specified layouts,
//! this reads a bidi_format value.

bidi_format to_bidi_directional_format(const ui::parser_lock &lock,
				       const char *element, const char *parent)
	LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads an halign value.
valign to_valign(const ui::parser_lock &lock,
		 const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a millimeter size value.
double to_mm(const ui::parser_lock &lock,
	     const char *element, const char *parent) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
