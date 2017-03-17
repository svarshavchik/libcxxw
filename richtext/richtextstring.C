/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "assert_or_throw.H"
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "fonts/current_fontcollection.H"
#include <x/sentry.H>
#include <algorithm>

LIBCXXW_NAMESPACE_START

richtextstring::richtextstring(const std::u32string &string,
			       const std::unordered_map<size_t,
			       richtextmeta> &meta)
	: string(string), meta(meta.begin(), meta.end())
{
	sort_and_validate(this->meta, string.size());
}

richtextstring::richtextstring(const richtextstring &other,
			       size_t pos,
			       size_t n)
	: string(other.string.substr(pos, n)), meta(other.meta)
{
	assert_or_throw(pos <= other.string.size() &&
			other.string.size()-pos >= n,
			"Internal error: invalid substring starting position or length.");

	if (n == 0)
	{
		meta.clear();
		return;
	}

	auto iter=duplicate(meta, pos+n);

	meta.erase(iter, meta.end());

	iter=duplicate(meta, pos);
	meta.erase(meta.begin(), iter);

	for (auto &m:meta)
		m.first -= pos;
	modified();
}

richtextstring::~richtextstring()=default;

void richtextstring::sort_and_validate(meta_t &meta, size_t n)
{
	std::sort(meta.begin(), meta.end(),
		  []
		  (const auto &a, const auto &b)
		  {
			  return a.first < b.first;
		  });

	assert_or_throw(n == 0 ? meta.empty():!meta.empty() &&
			meta.begin()->first == 0 &&
			(--meta.end())->first < n,
			"Internal error: metadata does not start with 0, or its last entry is not less than text size");

	auto b=meta.begin(), p=b, e=meta.end();

	if (b != e)
		while (++b != e)
		{
			assert_or_throw(b->first != p->first,
					"Internal error: duplicate metadata");
			p=b;
		}
}

void richtextstring::modified()
{
	coalesce_needed=true;
	fonts_need_resolving=true;
	resolved_fonts.clear();
}

void richtextstring::coalesce()
{
	if (!coalesce_needed)
		return;

	coalesce_needed=false;

	meta.erase(std::unique(meta.begin(), meta.end(),
			       []
			       (const auto &a,
				const auto &b)
			       {
				       return a.second == b.second;
			       }), meta.end());
}

class richtextstring::compare_meta_by_pos {
public:

	inline bool operator()(const meta_t::value_type &v, size_t p) const
	{
		return v.first < p;
	}

	inline bool operator()(size_t p, const meta_t::value_type &v) const
	{
		return p < v.first;
	}
};

richtextstring::meta_t::iterator
richtextstring::meta_lower_bound_by_pos(size_t p)
{
	return std::lower_bound(meta.begin(), meta.end(), p,
				compare_meta_by_pos());
}

richtextstring::meta_t::iterator
richtextstring::meta_upper_bound_by_pos(meta_t &meta, size_t p)
{
	return std::upper_bound(meta.begin(), meta.end(), p,
				compare_meta_by_pos());
}

richtextstring::meta_t::iterator richtextstring::duplicate(size_t pos)
{
	assert_or_throw(pos <= string.size(),
			"Internal error: metadata is empty");

	if (pos == string.size())
		return meta.end();

	return duplicate(meta, pos);
}

richtextstring::meta_t::iterator richtextstring::duplicate(meta_t &meta,
							   size_t pos)
{
	auto p=meta_upper_bound_by_pos(meta, pos);

	assert_or_throw(p != meta.begin(),
			"Internal error: metadata is empty");

	if (p[-1].first == pos)
		return --p; // Nothing to duplicate.

	auto copy=p[-1];

	copy.first=pos;

	return meta.insert(p, copy);
}

void richtextstring::insert(size_t pos,
			    const std::u32string &s,
			    const std::unordered_map<size_t,
			    richtextmeta> &smeta)
{
	meta_t new_meta(smeta.begin(), smeta.end());

	// smeta may be empty, to insert only text no metadata. Otherwise
	// smeta must be valid for the text string.
	if (!new_meta.empty())
		sort_and_validate(new_meta, s.size());

	if (s.empty())
		return; // Edge case, nothing to insert.

	insert(pos, s, new_meta);
}

void richtextstring::insert(size_t pos,
			    const richtextstring &s)
{
	meta_t meta_cpy=s.meta;

	insert(pos, s.string, meta_cpy);
}

void richtextstring::insert(size_t pos,
			    const std::u32string &s,
			    meta_t &new_meta)
{
	// Sanity check: insert point not past the end of the string.
	assert_or_throw(pos <= string.size(), "Internal error: bad position.");

	assert_or_throw((size_t)((size_t)(~0)-pos) > s.size(),
			"Internal error: overflow.");


	// Add insert position to the to-be-inserted metadata.
	for (auto &m:new_meta)
		m.first += pos;

	modified();

	// If an exception gets thrown, make sure we'll unwind everything.
	bool text_inserted=false;
	bool meta_inserted=false;

	auto cleanup=make_sentry
		([&, this]
		 {
			 // Unwind insert into the text string.

			 if (text_inserted)
			 {
				 auto b=this->string.begin()
					 +pos;

				 this->string.erase(b,
						    b+s.size());
			 }

			 // Unwind metadata insert.

			 if (meta_inserted)
			 {
				 auto &m=this->meta;

				 // Find what was inserted.
				 auto b=this->meta_lower_bound_by_pos(pos);
				 auto e=this->meta_lower_bound_by_pos(pos
								      +s.size()
								      );

				 auto p=m.erase(b, e);

				 // Reset back the adjusted positions.

				 auto ee=m.end();

				 while (p != ee)
				 {
					 p->first -= s.size();
					 ++p;
				 }
			 }
		 });
	cleanup.guard();

	// First, duplicate the metadata in effect at the insert position.

	auto meta_insert_iter=
		meta.empty() ? meta.end():duplicate(pos);

	for (auto p=meta_insert_iter; p != meta.end(); ++p)
		p->first += s.size();

	meta_inserted=true;
	meta.insert(meta_insert_iter, new_meta.begin(), new_meta.end());

	string.insert(string.begin()+pos,
		      s.begin(),
		      s.end());
	text_inserted=true;

	cleanup.unguard();
}

void richtextstring::erase(size_t pos, size_t n)
{
	assert_or_throw(pos <= string.size() && string.size()-pos >= n,
			"Internal error: overflow in erase()");

	if (n == 0)
		return;

	if (n == string.size())
	{
		clear();
		return;
	}

	modified();

	// Duplicate the metadata at the end of the range, then erase the
	// metadata in the range.

	auto e=duplicate(pos+n);
	auto b=meta_lower_bound_by_pos(pos);

	b=meta.erase(b, e);

	// Need to adjust the metadata position, to account for the removed
	// text.

	e=meta.end();
	while (b != e)
	{
		b->first -= n;
		++b;
	}

	// Now, erase the string.

	auto sb=string.begin()+pos;
	string.erase(sb, sb+n);
}

void richtextstring::clear()
{
	string.clear();
	meta.clear();
	modified();
}

void richtextstring::reserve(size_t n_chars, size_t n_meta)
{
	string.reserve(n_chars);
	meta.reserve(n_meta);
}

richtextstring &richtextstring::operator+=(const richtextstring &o)
{
	modified();

	string += o.string;

	size_t p=meta.size();

	meta.insert(meta.end(), o.meta.begin(), o.meta.end());

	for (auto b=meta.begin()+p, e=meta.end(); b != e; ++b)
		b->first += p;
	return *this;
}

void richtextstring::do_modify_meta(size_t start, size_t count,
				    const function<void (size_t,richtextmeta
							 &meta)> &l)
{
	modified();

	duplicate(start+count);

	auto iter=duplicate(start);

	for (auto e=meta.end(); iter != e; ++iter)
	{
		if (iter->first >= start+count)
			break;

		l(iter->first, iter->second);
	}
}

LIBCXXW_NAMESPACE_END
