/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontconfig_impl.H"
#include "fonts/fontobjectset_impl.H"
#include "fonts/fontpattern_impl.H"
#include "fonts/fontlist_impl.H"
#include "fonts/fontcharset_impl.H"
#include "x/w/font.H"

#include <x/exception.H>
#include <x/singleton.H>

#include <cmath>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

///////////////////////////////////////////////////////////////////////////////

configObj::configObj(const ref<implObj> &implArg) : impl(implArg)
{
}

configObj::~configObj()
{
}


objectset configObj::create_objectset(const std::set<std::string> &s)
{
	auto os=objectset::create(ref<objectsetObj::implObj>
				  ::create(config(this)));

	for (const auto &c:s)
		os->add(c);

	return os;
}

pattern configObj::create_pattern(const font &props, double dpi)
{
	auto p=create_pattern();

	if (!props.foundry.empty())
		p->add_string(FC_FOUNDRY, props.style);

	if (!props.family.empty())
		p->add_string(FC_FAMILY, props.family);

	if (props.weight >= 0)
		p->add_integer(FC_WEIGHT, props.weight);

	if (props.slant >= 0)
		p->add_integer(FC_SLANT, props.slant);

	if (props.width >= 0)
		p->add_integer(FC_WIDTH, props.width);

	if (!props.style.empty())
		p->add_string(FC_STYLE, props.style);

	if (props.pixel_size > 0)
		p->add_double(FC_PIXEL_SIZE, props.pixel_size);
	else if (props.point_size > 0)
		p->add_double(FC_PIXEL_SIZE,
			      std::round(dpi * props.point_size	 / 72));

	if (props.spacing >= 0)
		p->add_integer(FC_SPACING, props.spacing);

	p->substitute_matchpattern();
	p->substitute_defaults();

	return p;
}

pattern configObj::create_pattern()
{
	return pattern::create(ref<patternObj::implObj>::create(config(this)));
}

list configObj::create_list()
{
	return create_list(create_pattern());
}

list configObj::create_list(const pattern &patternArg)
{
	return create_list(patternArg,
			   create_objectset({
					   FC_FAMILY,
						   FC_FAMILYLANG,
						   FC_STYLE,
						   FC_STYLELANG,
						   FC_FULLNAME,
						   FC_FULLNAMELANG,
						   FC_SLANT,
						   FC_WEIGHT,
						   FC_SIZE,
						   FC_WIDTH,
						   FC_ASPECT,
						   FC_PIXEL_SIZE,
						   FC_SPACING,
						   FC_FOUNDRY,
						   FC_ANTIALIAS,
						   FC_HINTING,
						   FC_HINT_STYLE,
						   FC_VERTICAL_LAYOUT,
						   FC_AUTOHINT,
						   FC_FILE,
						   FC_INDEX,
						   FC_FT_FACE,
						   FC_OUTLINE,
						   FC_SCALABLE,
						   FC_SCALE,
						   FC_DPI,
						   FC_RGBA,
						   FC_LCD_FILTER,
						   FC_MINSPACE,
						   FC_CHARSET,
						   FC_LANG,
						   FC_FONTVERSION,
						   FC_CAPABILITY,
						   FC_FONTFORMAT,
						   FC_EMBOLDEN,
						   FC_EMBEDDED_BITMAP,
						   FC_DECORATIVE,
						   FC_FONT_FEATURES,
						   FC_NAMELANG,
						   FC_PRGNAME,
						   FC_POSTSCRIPT_NAME}));
}

list configObj::create_list(const pattern &patternArg,
			    const objectset &propertiesArg)
{
	return list::create(ref<listObj::implObj>
			    ::create(config(this), patternArg, propertiesArg));
}

charset configObj::create_charset()
{
	return charset::create(ref<charsetObj::implObj>::create(config(this)));
}

///////////////////////////////////////////////////////////////////////////////
class LIBCXX_HIDDEN configImplObj : public configObj {

 public:

	configImplObj() : configObj(ref<implObj>::create())
	{
	}

	~configImplObj()
	{
	}
};

static singleton<configImplObj> fontconfig_library;

config configBase::create()
{
	return fontconfig_library.get();
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
