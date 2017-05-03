/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_imagefwd_h
#define icon_imagefwd_h

#include <x/ptrfwd.H>
#include "x/w/types.H"

LIBCXXW_NAMESPACE_START

class icon_imageObj;

//! How to scale an icon.

//! Scale the icon size as close to the requested size as possible,
//! to be at least as big as the requested size, or not bigger than
//! the requested size.

enum class icon_scale { nearest, atleast, nomore};

//! An icon image.

//! An icon is a PICTURE of a pixmap that's loaded from a file.

typedef ref<icon_imageObj> icon_image;

//! A nullable pointer reference to an \ref icon "icon".

typedef ptr<icon_imageObj> icon_imageptr;

//! A reference to a constant \ref icon "icon".

typedef const_ref<icon_imageObj> const_icon_image;

//! A nullable pointer reference to a constant \ref icon "icon".

typedef const_ptr<icon_imageObj> const_icon_imageptr;


LIBCXXW_NAMESPACE_END

#endif
