/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_imageobj_H
#define icon_imageobj_H

#include "icon_imagefwd.H"
#include "x/w/picturefwd.H"
#include "x/w/pixmapfwd.H"

#include <string>
#include <unordered_map>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START;

//! An icon image object.

//! \see icon_image

class LIBCXX_HIDDEN icon_imageObj : virtual public obj {

public:

	//! The icon's picture
	const const_picture icon_picture;

	//! The icon's pixmap
	const const_pixmap  icon_pixmap;

	//! The icon's REPEAT render setting.
	const render_repeat repeat;

	//! Predefined points in an SXG image.

	//! The SXG image may define certain points. The
	//! locations of predefined points are stored here.
	const std::unordered_map<std::string, std::pair<coord_t, coord_t>
				 > points;
	//! Constructor.

	//! The constructor sets and saves the REPEAT render setting for this
	//! icon.
	icon_imageObj(const picture &icon_picture,
		      const pixmap &icon_pixmap,
		      render_repeat repeat,

		      //! Predefined points in the SXG image.

		      //! The SXG image may define certain points. The
		      //! locations of predefined points are stored here.
		      const std::unordered_map<std::string,
		      std::pair<coord_t, coord_t>> &points={});

	//! Destructor
	~icon_imageObj();
};

LIBCXXW_NAMESPACE_END

#endif
