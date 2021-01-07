/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "themeiconpixmapobj.H"
#include "x/w/impl/icon.H"
#include "drawable.H"
#include "pixmap.H"
#include "x/w/impl/pixmap_with_picture.H"
#include "x/w/dim_arg.H"

LIBCXXW_NAMESPACE_START

template<typename dim_type>
themeiconpixmapObj<dim_type>::~themeiconpixmapObj()=default;

template<typename dim_type>
icon themeiconpixmapObj<dim_type>::resize(ONLY IN_THREAD, dim_t w, dim_t h,
					  icon_scale scale)
{
	return this->image->impl->create_icon_pixels(this->name,
						     this->image->repeat,
						     w, h, scale);
}

template class themeiconpixmapObj<dim_arg>;

template class themeiconpixmapObj<dim_t>;

LIBCXXW_NAMESPACE_END
