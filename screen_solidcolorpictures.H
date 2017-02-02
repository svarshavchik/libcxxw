/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef screen_solidcolorpictures_h
#define screen_solidcolorpictures_h

#include "screen_solidcolorpicturesfwd.H"
#include "x/w/rgb.H"
#include "x/w/picturefwd.H"
#include <x/weakunordered_multimapfwd.H>
#include <x/obj.H>
#include <x/ref.H>

LIBCXXW_NAMESPACE_START

//! A cache of solid color pictures for this screen.

class LIBCXX_HIDDEN screen_solidcolorpicturesObj : virtual public obj {

 public:
	typedef x::weakunordered_multimap<rgb, pictureObj> map_t;

	map_t map;

	screen_solidcolorpicturesObj();

	~screen_solidcolorpicturesObj();
};

LIBCXXW_NAMESPACE_END

#endif
