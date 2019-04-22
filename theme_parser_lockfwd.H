/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef theme_parser_lockfwd_H
#define theme_parser_lockfwd_H

#include "x/w/namespace.H"
#include "x/w/types.H"
#include "x/w/alignment.H"
#include "x/w/defaultthemefwd.H"
#include <unordered_set>
#include <unordered_map>
#include <string>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN theme_parser_lock;

//! Read a single value.

//! When parsing XML-specified layouts,
//! this reads a required xpath.
std::string single_value(const theme_parser_lock &lock,
			 const char *element,
			 const char *parent) LIBCXX_HIDDEN;

//! Read an option value.

//! When parsing XML-specified layouts,
//! this reads an optional xpath if it exists, else returns an empty string.
std::string optional_value(const theme_parser_lock &lock,
			   const char *element,
			   const char *parent) LIBCXX_HIDDEN;


//! Read a single value, convert it to lowercase.

//! When parsing XML-specified layouts,
//! this reads a single_value() and converts it to lowercase.
std::string lowercase_single_value(const theme_parser_lock &lock,
				   const char *element,
				   const char *xpath) LIBCXX_HIDDEN;

//! Read a single value.

//! When parsing XML-specified layouts,
//! this checks whether the given xpath exists.
bool single_value_exists(const theme_parser_lock &lock,
			 const char *element) LIBCXX_HIDDEN;


//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a dim_t value.
dim_t to_dim_t(const theme_parser_lock &lock,
	       const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a size_t value.
size_t to_size_t(const theme_parser_lock &lock,
		 const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads a number between 0-100.
int to_percentage_t(const theme_parser_lock &lock,
		    const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads an halign value.
halign to_halign_value(const theme_parser_lock &lock,
		       const char *element, const char *parent) LIBCXX_HIDDEN;

//! Read a value.

//! When parsing XML-specified layouts,
//! this reads an halign value.
valign to_valign_value(const theme_parser_lock &lock,
		       const char *element, const char *parent) LIBCXX_HIDDEN;

//! Parse colors

//! Reads all <color> elements, filling in "colors". Any referenced colors
//! that are not found are placed into other_colors.

void parse_colors(const theme_parser_lock &root_lock,
		  std::unordered_map<std::string, theme_color_t> &colors,
		  std::unordered_set<std::string> &other_colors) LIBCXX_HIDDEN;

LIBCXXW_NAMESPACE_END

#endif
