/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_uixmlparser_h
#define x_w_uixmlparser_h

#include <x/w/namespace.H>
#include <variant>
#include <x/xml/doc.H>
#include <string>

LIBCXXW_NAMESPACE_START

namespace ui {
#if 0
}
#endif

//! Parsed <dim> value in the UI theme file

struct parsed_dim {
	//! Value of the scale attribute, if not empty.
	std::string scale;

	//! <dim>'s value.
	std::string value;
};

//! Load the values from a <dim>
parsed_dim parse_dim(const xml::doc::base::readlock &lock);

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
