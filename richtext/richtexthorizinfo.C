/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtexthorizinfo.H"
#include "assert_or_throw.H"

LIBCXXW_NAMESPACE_START

richtexthorizinfo_t::richtexthorizinfo_t()=default;
richtexthorizinfo_t::~richtexthorizinfo_t()=default;
richtexthorizinfo_t::richtexthorizinfo_t(const richtexthorizinfo_t &)=default;
richtexthorizinfo_t &richtexthorizinfo_t::operator=(const richtexthorizinfo_t &)
=default;

static void sanity_check(const richtexthorizinfo_t &cur,
			 size_t pos,
			 size_t len)
{
	assert_or_throw(pos < cur.size() && cur.size()-pos >= len,
			"Internal error: invalid parameters to richtexthorizinfo_t()");
}

richtexthorizinfo_t
::richtexthorizinfo_t(const richtexthorizinfo_t &cur,
	       size_t pos,
	       size_t len)
	: widths((sanity_check(cur, pos, len),
		  cur.widths.begin()+pos),
		 cur.widths.begin()+pos+len),
	  kernings(cur.kernings.begin()+pos,
		   cur.kernings.begin()+pos+len)
{
	updated();
}

void richtexthorizinfo_t::updated()
{
	assert_or_throw(widths.size() == kernings.size(),
			"Internal error, text fragment widths/kernings metadata not consistent");
}

void richtexthorizinfo_t::insert(size_t pos, size_t n_size)
{
	widths.insert(widths.begin()+pos, n_size, 0);
	kernings.insert(kernings.begin()+pos, n_size, 0);
	updated();
}

void richtexthorizinfo_t::erase(size_t pos, size_t n_size)
{
	widths.erase(widths.begin()+pos, widths.begin()+(pos+n_size));
	kernings.erase(kernings.begin()+pos, kernings.begin()+(pos+n_size));
	updated();
}

void richtexthorizinfo_t::append(const richtexthorizinfo_t &other)
{
	widths.reserve(other.size() + size());
	kernings.reserve(other.size() + size());

	widths.insert(widths.end(), other.widths.begin(),
			      other.widths.end());
	kernings.insert(kernings.end(), other.kernings.begin(),
			other.kernings.end());
	updated();
}

LIBCXXW_NAMESPACE_END
