/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_picturefwd_h
#define x_w_picturefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class pictureObj;
class pictureBase;

/*! A Picture object, as specified in the X Rendering extension.

A picture consists of a
\ref drawable "drawable", a \ref pictformat "pictformat", and
some rendering state, and implements rendering operations.

\code

INSERT_LIBX_NAMESPACE::w::picture dst_picture;

dst_picture->repeat(INSERT_LIBX_NAMESPACE::w::render_repeat::normal);

INSERT_LIBX_NAMESPACE::w::const_picture yellow=screen->create_solid_color_picture(
    { INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      0 });


dst_picture->composite(yellow, 0, 0, {0, 0, 100, 100},
                       INSERT_LIBX_NAMESPACE::w::render_pict_op::op_over);

INSERT_LIBX_NAMESPACE::w::rectangle_set rectangles;

dst_picture->clip_rectangles(rectangles);

dst_picture->clear_clip();

dst_picture->fill_rectangle( {0, 0, 100, 100}, { INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      INSERT_LIBX_NAMESPACE::w::rgb::maximum(),
      0 });

\endcode

*/
typedef ref<pictureObj, pictureBase> picture;

//! A constant picture

typedef const_ref<pictureObj, pictureBase> const_picture;

LIBCXXW_NAMESPACE_END

#endif
