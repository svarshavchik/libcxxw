/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/fontpattern_impl.H"
#include "fonts/fontcharset_impl.H"
#include "fonts/fontconfig_impl.H"
#include "fonts/fontsortedlist_impl.H"
#include "fonts/fontconfig.H"

#include <x/exception.H>
#include <algorithm>

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

patternObj::implObj::implObj(const config &configArg)
	: c(configArg), p(FcPatternCreate()), autodestroy(true)
{
	if (!*p_t::lock(p))
		throw EXCEPTION("Cannot create a new font pattern");

}

patternObj::implObj::implObj(const config &configArg,
			     FcPattern *pArg, bool autodestroyArg)
	: c(configArg), p(pArg), autodestroy(autodestroyArg)
{
}

patternObj::implObj::~implObj()
{
	if (autodestroy)
		FcPatternDestroy(*p_t::lock(p));
}


pattern patternObj::implObj::clone() const
{
	return pattern::create(ref<implObj>::create
			       (c,
				FcPatternDuplicate(*p_t::lock(p)),
				true));
}

std::string patternObj::implObj::unparse() const
{
	std::string s;

	p_t::lock lock(p);

	auto ret=FcNameUnparse(*lock);

	try {
		s=reinterpret_cast<const char *>(ret);
		free(ret);
	} catch (...)
	{
		free(ret);
		throw;
	}
	return s;
}

void patternObj::implObj::add_integer(const std::string_view &object, int val,
				      bool append)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);

	if (!append)
		FcPatternDel(*lock, object_name);
	FcPatternAddInteger(*lock, object_name, val);
}

void patternObj::implObj::add_double(const std::string_view &object, double val,
				     bool append)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);

	if (!append)
		FcPatternDel(*lock, object_name);
	FcPatternAddDouble(*lock, object_name, val);
}

void patternObj::implObj::add_bool(const std::string_view &object, bool val,
				   bool append)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);

	if (!append)
		FcPatternDel(*lock, object_name);
	FcPatternAddBool(*lock, object_name, val ? FcTrue:FcFalse);
}

void patternObj::implObj::add_string(const std::string_view &object,
				     const std::string_view &val,
				     bool append)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	FcChar8 val_buffer[val.size()+1];
	std::copy(val.begin(), val.end(), val_buffer);
	val_buffer[val.size()]=0;

	pattern_lock lock(this);

	if (!append)
		FcPatternDel(*lock, object_name);
	FcPatternAddString(*lock, object_name, val_buffer);
}

void patternObj::implObj::add_charset(const std::string_view &object,
				      const const_charset &val, bool append)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	auto copy=val->copy();

	pattern_lock lock(this);
	charsetObj::implObj::charset_t::lock charset{copy->impl->charset};

	if (!append)
		FcPatternDel(*lock, object_name);
	FcPatternAddCharSet(*lock, object_name, *charset);
}


bool patternObj::implObj::get_integer(const std::string_view &object,
				      int &value,
				      size_t index) const
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);

	if (FcPatternGetInteger(*lock, object_name, index, &value)
	    != FcResultMatch)
		return false;

	return true;
}

bool patternObj::implObj::get_double(const std::string_view &object, double &value,
				     size_t index) const
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);

	if (FcPatternGetDouble(*lock, object_name, index, &value)
	    != FcResultMatch)
		return false;

	return true;
}

bool patternObj::implObj::get_bool(const std::string_view &object, bool &value,
				   size_t index) const
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);
	FcBool v;

	if (FcPatternGetBool(*lock, object_name, index, &v)
	    != FcResultMatch)
		return false;

	value=v == FcTrue;
	return true;
}

bool patternObj::implObj::get_string(const std::string_view &object,
				     std::string &value,
				     size_t index) const
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);
	FcChar8 *v;

	if (FcPatternGetString(*lock, object_name, index, &v)
	    != FcResultMatch)
		return false;

	value=reinterpret_cast<const char *>(v);
	return true;
}

// Charset returned by get_charset() keeps a ref on the pattern, because
// it owns it.

class LIBCXX_HIDDEN patternCharsetObj : public charsetObj::implObj {

 public:
	patternCharsetObj(const ref<patternObj::implObj> &patternArg,
			  FcCharSet *charsetArg)
		: charsetObj::implObj(patternArg->c, charsetArg, false),
		pattern(patternArg)
		{
		}
	~patternCharsetObj()
	{
	}

	const ref<patternObj::implObj> pattern;
};

bool patternObj::implObj::get_charset(const std::string_view &object,
				      const_charsetptr &value, size_t index)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);
	FcCharSet *v;

	if (FcPatternGetCharSet(*lock, object_name, index, &v)
	    != FcResultMatch)
		return false;

	value=charset::create(ref<patternCharsetObj>::create(ref<implObj>
							     (this), v));
	return true;
}

bool patternObj::implObj::del(const std::string_view &object)
{
	char object_name[object.size()+1];
	std::copy(object.begin(), object.end(), object_name);
	object_name[object.size()]=0;

	pattern_lock lock(this);

	return FcPatternDel(*lock, object_name) == FcTrue;
}

void patternObj::implObj::substitute_matchpattern()
{
	pattern_lock lock(this);

	if (!FcConfigSubstitute(*fontconfig_t::lock{c->impl->fc},
				*lock, FcMatchPattern))
		throw EXCEPTION("FcConfigSubstitute failed");
}

void patternObj::implObj::substitute_matchfont()
{
	pattern_lock lock(this);

	if (!FcConfigSubstitute(*fontconfig_t::lock{c->impl->fc},
				*lock, FcMatchFont))
		throw EXCEPTION("FcConfigSubstitute failed");
}

void patternObj::implObj::substitute_matchscan()
{
	pattern_lock lock(this);

	if (!FcConfigSubstitute(*fontconfig_t::lock{c->impl->fc},
				*lock, FcMatchScan))
		throw EXCEPTION("FcConfigSubstitute failed");
}

void patternObj::implObj::substitute_defaults()
{
	pattern_lock lock(this);

	FcDefaultSubstitute(*lock);
}

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END
