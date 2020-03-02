/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef sxg_parserfwd_H
#define sxg_parserfwd_H

#include <x/ptrfwd.H>
#include "x/w/namespace.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN sxg_parserObj;

/*!
A parsed SXG file

This represents a parsed form of the contents of an SXG file.
An SXG file defines a scalable X-protocol graphic, or an icon.
The format of the SXG file maps directly onto X protocol and
render protocol requests.

default_width() and default_height() give the default size of the SXG
image, and depth() gives the depth, as specified in the SXG file.

width_for_mm() and height_for_mm() compute teh size of the SXG image,
in pixels, from millimeters.

adjust_width() and adjust_height() take a proposed width or height of
the scales SXG image, and returns a recommended width or height, nearest
to the proposed width or height, such that the scaled image will not show
any scaling artifacts.

height_for_width() takes a proposed width and returns a proposed height
of the image such as its aspect ratio is preserved. width_for_height()
takes a proposed height and returns a proposed width.

Once the size of the scaled image is determined, render() creates an
image for the given drawable, size, and options.

*/

typedef ref<sxg_parserObj> sxg_parser;

//! A nullable pointer reference to a \ref sxg_parser "parsed sxg file".

typedef ptr<sxg_parserObj> sxg_parserptr;

//! A reference to a const \ref sxg "parsed sxg file".

typedef const_ref<sxg_parserObj> const_sxg_parser;

//! A nullable pointer reference to a const \ref sxg "parsed sxg file".

typedef const_ptr<sxg_parserObj> const_sxg_parserptr;

LIBCXXW_NAMESPACE_END

#endif
