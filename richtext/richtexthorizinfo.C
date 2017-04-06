/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtexthorizinfo.H"
#include "assert_or_throw.H"
#include <algorithm>

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

	offsets_valid=false;
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

void richtexthorizinfo_t::compute_offsets()
{
	if (offsets_valid)
		return;

	offsets.clear();

	auto n=widths.size();
	assert_or_throw(n == kernings.size(),
			"Internal error, text fragment widths/kernings metadata not consistent");
	offsets.reserve(n);

	dim_t x=0;
	dim_t largest_x=0;

	for (auto i=n*0; i<n; ++i)
	{
		if (i)
			x=dim_t::truncate(x+kernings[i]);

		if (x > largest_x)
			largest_x=x;

		offsets.emplace_back(largest_x, x);

		x=dim_t::truncate(x+widths[i]);
	}
	offsets_valid=true;
}

size_t richtexthorizinfo_t::find_x_pos(dim_t xpos)
{
	compute_offsets();
	auto b=offsets.begin();
	auto iter=std::upper_bound(b, offsets.end(),
				   xpos,
				   []
				   (dim_t xpos, auto &pair)
				   {
					   return xpos < pair.first;
				   });

	assert_or_throw(iter != b,
			"upper_bound() should not have returned begin()");


	if (xpos < dim_t::truncate(iter[-1].second + widths.at(iter-b-1)))
		--iter;

	return iter-b;
}

dim_t richtexthorizinfo_t::x_pos(size_t i)
{
	compute_offsets();

	assert_or_throw(i < offsets.size(), "Invalid value for x_pos");

	return offsets[i].second;
}


LIBCXXW_NAMESPACE_END
