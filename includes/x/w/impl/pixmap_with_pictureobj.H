/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_pixmap_with_pictureobj_h
#define x_w_impl_pixmap_with_pictureobj_h

#include <x/w/impl/pixmap_with_picturefwd.H>
#include <x/w/pixmapobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see pixmap_with_picture

class LIBCXX_HIDDEN pixmap_with_pictureObj : public pixmapObj {

public:

	pixmap_with_pictureObj(const ref<implObj> &pixmap_impl,
			       render_repeat repeat);

	~pixmap_with_pictureObj();

	//! Our picture's \c render_repeat
	const render_repeat repeat;

	//! Our picture
	const picture icon_picture;

	//! Override create_picture()

	//! Return our cached picture.
	const_picture create_picture() const override;
};

LIBCXXW_NAMESPACE_END

#endif
