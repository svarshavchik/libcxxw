/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_gc_H
#define x_w_gc_H

#include <x/ref.H>
#include <x/w/gcfwd.H>
#include <x/w/gcobj.H>
#include <x/w/namespace.H>

#include <string_view>

LIBCXXW_NAMESPACE_START

//! Base class for \ref gc "graphic context"s.

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base

class gcBase : public ptrref_base {
public:
	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::line
	typedef gcObj::line line;

	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::polyline
	typedef gcObj::polyline polyline;

	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::arc

	typedef gcObj::arc arc;

	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::fill_arc_mode
	typedef gcObj::fill_arc_mode fill_arc_mode;

	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::properties

	typedef gcObj::properties properties;

	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::function
	typedef gcObj::function function;

	//! Convenience typedef

	//! Refer to this class as \c INSERT_LIBX_NAMESPACE::w::gc::base::line_style
	typedef gcObj::line_style line_style;

	//! Convert a mode value to a string
	static std::string function_to_string(function);

	//! Convert a mode value from a string
	static function function_from_string(const std::string_view &s);

	//! Convert a line_style value to a string
	static std::string line_style_to_string(line_style);

	//! Convert a line_style value from a string
	static line_style line_style_from_string(const std::string_view &s);

	//! Convert a fill_arc_mode value to a string
	static std::string fill_arc_mode_to_string(fill_arc_mode);

	//! Convert a fill_arc_mode value from a string
	static fill_arc_mode fill_arc_mode_from_string(const std::string_view &s);

	//! Convert a cap_style value to a string
	static std::string cap_style_to_string(cap_style);

	//! Convert a cap_style value from a string
	static cap_style cap_style_from_string(const std::string_view &s);

	//! Convert a join_style value to a string
	static std::string join_style_to_string(join_style);

	//! Convert a join_style value from a string
	static join_style join_style_from_string(const std::string_view &s);

	//! Convenience typedef

	typedef gcObj::polyfill polyfill;
};

LIBCXXW_NAMESPACE_END

#endif
