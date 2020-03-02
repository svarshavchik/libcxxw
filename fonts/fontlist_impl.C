/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontlist_impl.H"
#include "fonts/fontpattern_impl.H"
#include "fonts/fontconfig_impl.H"
#include "fonts/fontconfig.H"
#include "fonts/fontobjectset_impl.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

listObj::implObj::implObj(const config &configArg)
	: c(configArg), s(FcFontSetCreate()), autodestroy(true)
{
	if (!*s_t::lock(s))
		throw EXCEPTION("Cannot create a new font list");
}

listObj::implObj::implObj(const config &configArg,
			  const pattern &patternArg,
			  const objectset &objectsetArg)
	: c(configArg), s( ({
				patternObj::implObj::p_t::lock
					lock(patternArg->impl->p);

				objectsetObj::implObj::set_t::lock
					objectset_lock{objectsetArg->impl->set};
				FcFontList(*fontconfig_t::lock{c->impl->fc},
					   *lock,
					   *objectset_lock);

			})),
	  autodestroy(true)
{
	if (!*s_t::lock{s})
		throw EXCEPTION("Cannot create a font list");
}

listObj::implObj::implObj(const config &configArg,
			  FcFontSet *sArg, bool autodestroyArg)
	: c(configArg), s(sArg), autodestroy(autodestroyArg)
{
	if (!*s_t::lock(s))
		throw EXCEPTION("Cannot create a font list");
}

listObj::implObj::~implObj()
{
	if (autodestroy)
	{
		s_t::lock lock(s);
		FcFontSetDestroy(*lock);
	}
}

ref<patternObj::implObj> listObj::implObj::get_pattern(size_t index)
{
	implObj::s_t::lock lock(s);

	if (index >= (size_t)(*lock)->nfont)
		throw EXCEPTION("Font list iterator out of bounds");

	return get_pattern(*lock, index);
}

// When return a pattern object from a list, keep an internal ref to the list,
// as long as the pattern object remains in existence.

class LIBCXX_HIDDEN listPatternObj : public patternObj::implObj {

 public:

	listPatternObj(const ref<listObj::implObj> &listArg,
		       FcPattern *pArg)
		: patternObj::implObj(listArg->c, pArg, false),
		list(listArg)
		{
		}
	~listPatternObj()=default;

	const ref<listObj::implObj> list;
};

ref<patternObj::implObj> listObj::implObj::get_pattern(FcFontSet *fs,
						       size_t index)
{
	return ref<listPatternObj>::create(ref<implObj>(this),
					   fs->fonts[index]);
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
