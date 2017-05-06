/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef icon_imageobj_H
#define icon_imageobj_H

#include "icon_imagefwd.H"
#include "x/w/picturefwd.H"
#include "x/w/pixmapfwd.H"

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

	//! Constructor.

	//! The constructor sets and saves the REPEAT render setting for this
	//! icon.
	icon_imageObj(const picture &icon_picture,
		      const pixmap &icon_pixmap,
		      render_repeat repeat);

	//! Destructor
	~icon_imageObj();
};

LIBCXXW_NAMESPACE_END

#endif
