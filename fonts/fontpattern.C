/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontpattern_impl.H"
#include "fonts/fontcharset_impl.H"
#include "fonts/fontconfig_impl.H"
#include "fonts/fontsortedlist_impl.H"
#include "fonts/fontconfig.H"
#include "screen.H"
#include "defaulttheme.H"

#include <x/exception.H>
#include <cmath>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

patternObj::patternObj(const ref<implObj> &implArg) : impl(implArg)
{
}

patternObj::~patternObj()
{
}

pattern patternObj::clone() const
{
	return impl->clone();
}

std::string patternObj::unparse() const
{
	return impl->unparse();
}

void patternObj::add_integer(const std::string_view &object, int val, bool append)
{
	impl->add_integer(object, val, append);
}

void patternObj::add_double(const std::string_view &object, double val, bool append)
{
	impl->add_double(object, val, append);
}

void patternObj::add_bool(const std::string_view &object, bool val, bool append)
{
	impl->add_bool(object, val, append);
}

void patternObj::add_string(const std::string_view &object,
			    const std::string_view &val,
			    bool append)
{
	impl->add_string(object, val, append);
}

void patternObj::add_charset(const std::string_view &object,
			     const const_charset &val, bool append)
{
	impl->add_charset(object, val, append);
}


bool patternObj::get_integer(const std::string_view &object, int &value,
			     size_t index) const
{
	return impl->get_integer(object, value, index);
}

bool patternObj::get_double(const std::string_view &object, double &value,
			    size_t index) const
{
	return impl->get_double(object, value, index);
}

bool patternObj::get_bool(const std::string_view &object, bool &value,
			  size_t index) const
{
	return impl->get_bool(object, value, index);
}

bool patternObj::get_string(const std::string_view &object, std::string &value,
			    size_t index) const
{
	return impl->get_string(object, value, index);
}

bool patternObj::get_charset(const std::string_view &object,
			     const_charsetptr &value, size_t index) const
{
	return impl->get_charset(object, value, index);
}

bool patternObj::del(const std::string_view &object)
{
	return impl->del(object);
}

void patternObj::substitute_matchpattern()
{
	impl->substitute_matchpattern();
}

void patternObj::substitute_matchfont()
{
	impl->substitute_matchfont();
}

void patternObj::substitute_matchscan()
{
	impl->substitute_matchscan();
}

void patternObj::substitute_defaults()
{
	impl->substitute_defaults();
}

pattern_lock::pattern_lock(const patternObj::implObj *me)
	: patternObj::implObj::p_t::lock(me->p)
{
}

pattern_lock::~pattern_lock()
{
}

sortedlist patternObj::match(bool trim)
{
	pattern_lock lock(&*impl);

	FcCharSet *cs=nullptr;
	FcResult res;
	FcFontSet *fs=FcFontSort(*fontconfig_t::lock{impl->c->impl->fc},
				 *lock, trim, &cs, &res);

	if (!cs)
	{
		if (fs)
			FcFontSetDestroy(fs);
		throw EXCEPTION("Could not create FcCharset");
	}

	charset csp=charset::create(ref<charsetObj::implObj>
				    ::create(impl->c, cs, true));

	return sortedlist::create(ref<sortedlistObj::implObj>
				  ::create(impl->c, fs, csp,
					   pattern(this)));
}

size_t patternObj::get_point_size(const ref<screenObj::implObj> &screen)
	const
{
	auto theme=screen->current_theme.get();

	double v=0;

	if (get_double(FC_PIXEL_SIZE, v))
	{
		// height in pixels / height in millimeters
		// = pixels per millimeter
		//
		// (multiply by themescale, of course)
		//
		// v is number of pixels. Divide by pixels per
		// millimeter gives number of millimeters.
		//
		// number of millimeters / 25.4 * 72 = point
		// size.

		v = v / ((dim_t::value_type)(screen->height_in_pixels())
			 * theme->themescale /(dim_t::value_type)
			 (screen->height_in_millimeters()))
			* (72.0/25.4);
	}

	return std::round(v);
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
