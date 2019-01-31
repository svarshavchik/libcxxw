/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_image_param_H
#define x_w_image_param_H

#include <string>
#include <x/w/image_paramfwd.H>

LIBCXXW_NAMESPACE_START

//! A list item that's an image.

//! \code
//! ll->append_items(INSERT_LIBX_NAMESPACE::w::image_param{"bullet2.sxg"});
//! \endcode
//!
//! Passed to \ref listlayoutmanager "listlayoutmanager"'s
//! insert_items(), and append_items(), to indicate an image list item.
//!
//! Supports jpg, gif, png, and sxg image formats. The filename gets loaded
//! from the current directory or from the current display theme's directory.
//! If the specified filename does not have an extension, the first file
//! found with a known extension gets loaded.

struct image_param : public std::string {

	using std::string::string;
};

//! "foo"_image specifies an image icon to be used in a list.

//! \see image_param

inline image_param operator"" _image(const char *p, size_t n)
{
	return {p, p+n};
}

LIBCXXW_NAMESPACE_END

#endif
