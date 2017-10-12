/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_image_param_H
#define x_w_image_param_H

#include <string>
#include <x/w/image_paramfwd.H>

LIBCXXW_NAMESPACE_START

//! A cell list item that's an image.

struct image_param : public std::string {

	using std::string::string;
};

//! "foo"_image specifies an image icon to be used in a list.

//! Passed to \ref textlistlayoutmanager "textlistlayoutmanager"'s
//! insert_item(), and append_item(), to indicate an image list item.

inline image_param operator"" _image(const char *p, size_t n)
{
	return {p, p+n};
}

LIBCXXW_NAMESPACE_END

#endif
