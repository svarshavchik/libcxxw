/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef sxg_imageobj_H
#define sxg_imageobj_H

#include "sxg/sxg_imagefwd.H"
#include "icon_imageobj.H"
#include "x/w/picturefwd.H"
#include "x/w/pixmapfwd.H"
#include "x/w/types.H"

#include <string>
#include <unordered_map>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START;

class LIBCXX_HIDDEN sxg_imageObj : public icon_imageObj {

public:

	//! Constructor
	sxg_imageObj(const picture &rendered_picture,
		     const pixmap  &rendered_pixmap,
		     render_repeat repeat);

	//! Destructor
	~sxg_imageObj();

	//! Predefined points in the SXG image.

	//! The SXG image may define certain points. The
	//! locations of predefined points are stored here.
	std::unordered_map<std::string,
			   std::pair<coord_t, coord_t>> points;

};

LIBCXXW_NAMESPACE_END

#endif
