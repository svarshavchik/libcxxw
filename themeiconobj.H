/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef themeiconobj_H
#define themeiconobj_H

#include "iconobj.H"
#include "defaultthemefwd.H"

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
	const defaulttheme theme;

	//! Original requested width
	const dim_type width;

	//! Original requested height
	const dim_type height;

	//! Original scaling hint

	const icon_scale scale;

	//! Constructor
	themeiconObj(const std::string &name,
		     const defaulttheme &theme,
		     const dim_type &width,
		     const dim_type &height,
		     icon_scale scale,
		     const const_icon_image &image);

	//! Destructor
	~themeiconObj();

	//! Override initialize.
	icon initialize(IN_THREAD_ONLY) override;

	//! Override theme_updated.

	icon theme_updated(IN_THREAD_ONLY,
			   const defaulttheme &new_theme) override;
};

LIBCXXW_NAMESPACE_END

#endif