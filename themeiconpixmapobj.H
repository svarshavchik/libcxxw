/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themeiconpixmapobj_H
#define themeiconpixmapobj_H

#include "themeiconobj.H"

LIBCXXW_NAMESPACE_START;

//! An icon that's based on a loaded jpg, gif, or a png image.

//! The template parameter is either dim_arg, or dim_t, indicating how the
//! size of the icon is specified.
//!
//! Derived from themeiconObj, and implements resizemm() and resize.

template<typename dim_type>
class LIBCXX_HIDDEN themeiconpixmapObj : public themeiconObj<dim_type> {

 public:

	using themeiconObj<dim_type>::themeiconObj;

	~themeiconpixmapObj();

	//! Override resiz().

	//! Won't resize anything, but we need to construct a new cached icon
	//! for the new virtual size.
	icon resize(ONLY IN_THREAD, dim_t w, dim_t h,
		    icon_scale scale) override;

};

LIBCXXW_NAMESPACE_END

#endif
