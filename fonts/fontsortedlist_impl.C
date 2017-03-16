/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontsortedlist_impl.H"
#include "fonts/fontconfig_impl.H"
#include "fonts/fontcharset.H"
#include "fonts/fontpattern_impl.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

sortedlistObj::implObj::implObj(const config &configArg,
				FcFontSet *sArg,
				const charset &csArg,
				const pattern &patArg)
	: listObj::implObj(configArg, sArg, true),
	  cs(csArg),
	  pat(patArg)
{
}

sortedlistObj::implObj::~implObj()
{
}

ref<patternObj::implObj> sortedlistObj::implObj::get_pattern(FcFontSet *fs,
							     size_t index)
{
	return ref<patternObj::implObj>
		::create(c,
			 FcFontRenderPrepare
			 (*fontconfig_t::lock(c->impl->fc),
			  *patternObj::implObj::p_t::lock(pat->impl->p),
			  fs->fonts[index]),
			 true);
}


#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
