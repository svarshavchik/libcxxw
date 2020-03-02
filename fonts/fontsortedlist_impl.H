/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontsortedlist_impl_H
#define fontsortedlist_impl_H

#include "fonts/fontconfigfwd.H"
#include "x/w/impl/fonts/fontcharsetfwd.H"
#include "fonts/fontpatternfwd.H"
#include "fonts/fontlist_impl.H"
#include "fonts/fontsortedlist.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! \ref fontsortedlist "fontsortedlist" internal implementation object.

//! This is a subclass of a \ref fontlist "fontlist", that overrides
//! get_pattern() to invoke FcFontRenderPrepare() on the font pattern.

class sortedlistObj::implObj : public listObj::implObj {

public:
	//! Constructor

	implObj(const config &configArg,
		FcFontSet *sArg,
		const charset &csArg,
		const pattern &patArg);

	//! Destructor
	~implObj();

	//! Maintain a reference to the charset used to create this list.
	const charset cs;

	//! Maintain a reference to the pattern used to create this list.
	const pattern pat;

	//! Override get_pattern().
	ref<patternObj::implObj> get_pattern(FcFontSet *fs,
					     size_t index) override;
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
