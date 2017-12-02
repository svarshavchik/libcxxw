/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themeiconpixmapobj_H
#define themeiconpixmapobj_H

#include "themeiconobj.H"
#include <string>

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

	//! Override resizemm().

	//! Won't resize anything, but we need to construct a new cached icon
	//! for the new virtual size.

	icon resizemm(IN_THREAD_ONLY,
		      const dim_arg &width_arg,
		      const dim_arg &height_arg) override;

	//! Override resiz().

	//! Won't resize anything, but we need to construct a new cached icon
	//! for the new virtual size.
	icon resize(IN_THREAD_ONLY, dim_t w, dim_t h,
		    icon_scale scale) override;

};

LIBCXXW_NAMESPACE_END

#endif
