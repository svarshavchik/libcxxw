/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themeiconobj_H
#define themeiconobj_H

#include "x/w/impl/iconobj.H"
#include "x/w/defaultthemefwd.H"
#include "x/w/impl/pixmap_with_picturefwd.H"
#include <string>

LIBCXXW_NAMESPACE_START;

//! A theme-based icon

//! The template parameter is either dim_arg, or dim_t, indicating how the
//! size of the icon is specified.
//!
//! Derives from iconObj. Implements initialize() and theme_updated().

template<typename dim_type>
class LIBCXX_HIDDEN themeiconObj : public iconObj {

 public:

	//! Name
	const std::string name;

	//! Current theme
	const const_defaulttheme theme;

	//! Original requested width
	const dim_type width;

	//! Original requested height
	const dim_type height;

	//! Original scaling hint

	const icon_scale scale;

	//! Constructor
	themeiconObj(const std::string &name,
		     const const_defaulttheme &theme,
		     const dim_type &width,
		     const dim_type &height,
		     icon_scale scale,
		     const pixmap_with_picture &pixmap);

	//! Destructor
	~themeiconObj();

	//! Override initialize.
	icon initialize(ONLY IN_THREAD) override;

	//! Override theme_updated.

	icon theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme) override;
};

LIBCXXW_NAMESPACE_END

#endif
