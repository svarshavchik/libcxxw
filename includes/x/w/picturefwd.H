/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_picturefwd_h
#define x_w_picturefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_PUBLIC pictureObj;
class LIBCXX_PUBLIC pictureBase;

/*! A Picture object, as specified in the X Rendering extension.

A picture consists of a
\ref drawable "drawable", a \ref pictformat "pictformat", and
some rendering state, and implements rendering operations.

This class implements Picture object operations, see the RENDER X
protocol extension for more information.

\code
INSERT_LIBX_NAMESPACE::w::picture dst_picture;

dst_picture->repeat(INSERT_LIBX_NAMESPACE::w::render_repeat::normal);

INSERT_LIBX_NAMESPACE::w::const_picture yellow=screen->create_solid_color_picture(
    { INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      0 });


dst_picture->composite(yellow, 0, 0, {0, 0, 100, 100},
                       INSERT_LIBX_NAMESPACE::w::render_pict_op::op_over);

INSERT_LIBX_NAMESPACE::w::rectarea rectangles;

dst_picture->clip_rectangles(rectangles);

dst_picture->clear_clip();

dst_picture->fill_rectangle( {0, 0, 100, 100}, { INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      0 });

std::set<INSERT_LIBX_NAMESPACE::w::picture::base::triangle> triangles;

dst_picture->fill_triangles(triangles, yellow);

std::vector<INSERT_LIBX_NAMESPACE::w::picture::base::point> points;

dst_picture->fill_tri_strip(points, yellow);

dst_picture->fill_tri_fan(points, yellow);

\endcode

These picture methods implement various compositing operations. Several
overloaded methods offer alternative calling conventions, and options,
like specifying an explicit compositing operation (the default is SRC).

*/
typedef ref<pictureObj, pictureBase> picture;

//! A constant picture

typedef const_ref<pictureObj, pictureBase> const_picture;

LIBCXXW_NAMESPACE_END

#endif
