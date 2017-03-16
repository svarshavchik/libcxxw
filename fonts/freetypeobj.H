/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef freetypeobj_h
#define freetypeobj_H

#include <x/obj.H>
#include "x/w/screenfwd.H"
#include "fonts/fontpatternfwd.H"
#include "x/w/types.H"

#include "fonts/freetypefwd.H"
#include "fonts/cached_fontfwd.H"
#include <ft2build.h>
#include FT_FREETYPE_H

LIBCXXW_NAMESPACE_START

//! Freetype 2 wrapper.

//! Freetype is not thread safe. We slap a bunch of locks on it.

class LIBCXX_HIDDEN freetypeObj : virtual public obj {

	LOG_CLASS_SCOPE;

 public:

	//! The freetype library is only thread safe across different
	//! FT_Library instances. Only a single thread at a time can
	//! access any object that's related to a single FT_Library,
	//! so we'll need to be careful.

	typedef mpobj<FT_Library, std::recursive_mutex> library_t;

	//! The freetype library handle, mutex protected.
	mutable library_t library;

	template<typename obj_type> class ft_object;

	//! Constructor
	freetypeObj();

	//! Destructor
	~freetypeObj();

#if 0
	freetypefontptr font(const const_drawable &drawable,
			     const fontconfig::const_pattern &pattern)
		const override;
#endif

	//! Return a cached \ref cached_font "cached font".

	cached_fontptr font(const const_screen &screen,
			   depth_t depth,
			   const fontconfig::const_pattern &pattern) const;

	class ftbitmap;
};

LIBCXXW_NAMESPACE_END

#endif
