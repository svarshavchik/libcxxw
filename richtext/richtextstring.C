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
	modified();
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

	if (!other.fonts_need_resolving)
	{
		// We can save ourselves a lot of work by copying these, too.

		auto b=std::upper_bound
			(other.resolved_fonts.begin(),
			 other.resolved_fonts.end(),
			 pos,
			 []
			 (size_t pos, const auto &pair)
			 {
				 return pos < pair.first;
			 });

		if (b == other.resolved_fonts.begin())
			throw EXCEPTION("Internal error: invalid resolved_fonts cache");
		--b;

		auto e=std::lower_bound
			(other.resolved_fonts.begin(),
			 other.resolved_fonts.end(),
			 pos+n,
			 []
			 (const auto &pair, size_t pos)
			 {
				 return pair.first < pos;
			 });

		if (!(b < e))
			throw EXCEPTION("Internal error: invalid resolved_fonts cache");
		resolved_fonts.reserve(e-b);

		std::transform(b, e,
			       std::back_insert_iterator<resolved_fonts_t>
			       (resolved_fonts),
			       [pos]
			       (const auto &pair)
			       {
				       auto cpy=pair;

				       cpy.first=cpy.first < pos ? 0:
					       cpy.first-pos;
				       return cpy;
			       });
		fonts_need_resolving=false;
	}
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
	// NOTE: insert() and erase() adjusts resolved_fonts, and only sets
	// coalesce_needed, by itself.

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
			    const std::u32string_view &s)
{
	if (s.size() == 0)
		return;

	// Find the metadata at the given position, and use it for this insert.

	if (string.empty())
		throw EXCEPTION("Internal error: Non-metadata insert into an empty string.");

	auto p=std::upper_bound(meta.begin(),
				meta.end(),
				pos,
				[]
				(size_t pos, const auto &pair)
				{
					return pos < pair.first;
				});

	if (p == meta.begin())
		throw EXCEPTION("Internal error: bad metadata.");
	--p;

	meta_t new_meta{ {0, p->second} };

	modified();

	do_insert(pos, s, new_meta);
}

void richtextstring::insert(size_t pos, richtextstring s)
{
	if (s.size() == 0)
		return;

	if (size() == 0)
	{
		*this=s;
		return; // Edge case.
	}

	bool fonts_not_resolved =
		fonts_need_resolving || s.fonts_need_resolving;

	do_insert(pos, s.string, s.meta);

	if (fonts_not_resolved)
	{
		modified();
		return;
	}
	coalesce_needed=true;

	// We can save ourselves some work resolving fonts by logically
	// inserting the other string's resolved fonts.

	resolved_fonts.reserve(resolved_fonts.size()
			       + s.resolved_fonts.size() + 1);

	// Find the insert position, insert the other string's resolved fonts
	// ...

	auto insert_pos=std::upper_bound
		(resolved_fonts.begin(),
		 resolved_fonts.end(),
		 pos,
		 []
		 (size_t pos, const auto &pair)
		 {
			 return pos < pair.first;
		 });

	assert_or_throw(insert_pos != resolved_fonts.begin(),
			"Internal error: corrupted resolved_fonts");

	// The C++ standard is too harsh:
	//
	// "If no reallocation happens, all the iterators and references
	// before the insertion point remain valid.
	//
	// So, if we insert() here, insert_pos will be technically invalid.

	size_t insert_idx=insert_pos-resolved_fonts.begin();

	if (insert_pos[-1].first == pos)
	{
		--insert_pos;
		--insert_idx;
	}
	else
	{
		auto cpy= insert_pos[-1];
		cpy.first=pos;

		resolved_fonts.insert(insert_pos, cpy);

		insert_pos=resolved_fonts.begin() + insert_idx;
	}

	resolved_fonts.insert(insert_pos,
			      s.resolved_fonts.begin(),
			      s.resolved_fonts.end());

	insert_pos=resolved_fonts.begin()+insert_idx;

	// ... and adjust the inserted indexes accordingly.
	auto after_insert_pos=insert_pos+s.resolved_fonts.size();

	std::for_each(insert_pos, after_insert_pos,
		      [pos]
		      (auto &pair)
		      {
			      pair.first += pos;
		      });

	std::for_each(after_insert_pos, resolved_fonts.end(),
		      [len=s.size()]
		      (auto &pair)
		      {
			      pair.first += len;
		      });

	// We now need to check both ends of the inserted string, to see if
	// coalescing is needed.
	//
	// Although we know how vector iterators work, the C++ standard is
	// a bit harsh: all iterators on or after the erase() point get
	// invalidated. It's overkill, but let's be strictly conforming.

	size_t insert_index=insert_pos-resolved_fonts.begin();
	size_t after_insert_idx=after_insert_pos-resolved_fonts.begin();

	while (insert_index > 0 &&
	       (insert_pos[-1].first == insert_pos->first ||
		insert_pos[-1].second == insert_pos->second))
	{
		--insert_pos;

		auto saved_pos=insert_pos->first;
		resolved_fonts.erase(insert_pos);

		insert_pos=resolved_fonts.begin()+(--insert_index);
		insert_pos->first=saved_pos;
		--after_insert_idx;
	}
	after_insert_pos=resolved_fonts.begin()+after_insert_idx;

	while (after_insert_idx > 0 &&
	       after_insert_pos != resolved_fonts.end() && // Shouldn't happen
	       (after_insert_pos[-1].first == after_insert_pos->first ||
		after_insert_pos[-1].second == after_insert_pos->second))
	{
		--after_insert_pos;
		--after_insert_idx;

		auto saved_pos=after_insert_pos->first;
		resolved_fonts.erase(after_insert_pos);
		after_insert_pos=resolved_fonts.begin()+after_insert_idx;
		after_insert_pos->first=saved_pos;
	}
}

void richtextstring::do_insert(size_t pos,
			       const std::u32string_view &s,
			       meta_t &new_meta)
{
	// Sanity check: insert point not past the end of the string.
	assert_or_throw(pos <= string.size(), "Internal error: bad position.");

	assert_or_throw((size_t)((size_t)(~0)-pos) > s.size(),
			"Internal error: overflow.");


	// Add insert position to the to-be-inserted metadata.
	for (auto &m:new_meta)
		m.first += pos;

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

	coalesce_needed=true;

	if (!fonts_need_resolving)
	{
		// Adjust resolved font cache.

		resolved_fonts.reserve(resolved_fonts.size()+1);

		auto after_erased=std::upper_bound
			(resolved_fonts.begin(),
			 resolved_fonts.end(),
			 pos+n,
			 []
			 (size_t pos, const auto &pair)
			 {
				 return pos < pair.first;
			 });

		assert_or_throw(after_erased != resolved_fonts.begin(),
				"Internal error: corrupted resolved_fonts");

		if ((--after_erased)->first != pos+n)
		{
			resolved_fonts.insert(after_erased+1,
					      {pos+n, after_erased->second});
			++after_erased;
		}

		auto before_erased=std::lower_bound
			(resolved_fonts.begin(),
			 resolved_fonts.end(),
			 pos,
			 []
			 (const auto &pair, size_t pos)
			 {
				 return pair.first < pos;
			 });

		size_t before_index=before_erased-resolved_fonts.begin();

		resolved_fonts.erase(before_erased, after_erased);

		before_erased=resolved_fonts.begin()+before_index;

		std::for_each(before_erased, resolved_fonts.end(),
			      [n]
			      (auto &pair)
			      {
				      pair.first -= n;
			      });

		while (before_erased > resolved_fonts.begin() &&
		       before_erased[-1].second == before_erased->second)
		{
			--before_erased;

			resolved_fonts.erase(before_erased+1);
		}
	}

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
