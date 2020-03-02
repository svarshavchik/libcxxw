/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontlist_impl_H
#define fontlist_impl_H

#include "fonts/fontconfigfwd.H"
#include "fonts/fontlist.H"
#include "fonts/fontpattern.H"
#include "fonts/fontpatternobj.H"
#include "fonts/fontobjectsetfwd.H"
#include <x/mpobj.H>
#include <fontconfig/fontconfig.h>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! Implementation object.

class LIBCXX_HIDDEN listObj::implObj : virtual public obj {

public:
	//! Create a new, empty FcFontSet
	implObj(const config &configArg);

	//! Create an FcFontSet for the specified pattern.

	implObj(const config &configArg,
		const pattern &patternArg,
		const objectset &objectsetArg);

	//! Create a wrapper for someone else's FcFontSet.

protected:
	implObj(const config &configArg,
		FcFontSet *sArg,
		bool autodestroyArg);
public:
	//! Destructor
	~implObj();

	//! My fontconfig library.
	const config c;

	//! A thread-safe container for a FcFontSet

	typedef mpobj<FcFontSet * const> s_t;

	//! The font list.

	s_t s;

	//! Whether we own the FcFontSet

	//! This FcFontSet could belong to another object, in which case the
	//! destructor will not destroy it.
	const bool autodestroy;

	//! Extract pattern #index from the FcFontSet

	ref<patternObj::implObj> get_pattern(size_t index);

	//! Overridden in fontsortedlist
	virtual ref<patternObj::implObj> get_pattern(FcFontSet *fs,
						     size_t index);
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
