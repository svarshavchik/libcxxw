/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "assert_or_throw.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/text_hotspot.H"
#include <x/sentry.H>
#include <x/locale.H>
#include <algorithm>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

void richtextstring::finish_from_string()
{
	sort_and_validate(this->meta, this->string.size());
	modified();
}

richtextstring::richtextstring(const richtextstring &other,
			       size_t pos,
			       size_t n)
	: string{other.string.substr(pos, n)}, meta{other.meta}
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

bool richtextstring::operator==(const richtextstring &s) const
{
	return get_string() == s.get_string() &&
		get_meta() == s.get_meta();
}

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

void richtextstring::shrink_to_fit()
{
	coalesce();
	meta.shrink_to_fit();
	string.shrink_to_fit();
}

void richtextstring::coalesce() const
{
	if (!coalesce_needed)
		return;

	coalesce_needed=false;
	cached_dir=richtext_dir::lr;

	auto p=meta.begin(), e=meta.end();

	auto q=p;

	// This is equivalent to std::unique, but we take advantage of
	// the opportunity to also figure out our direction.

	bool has_lr=false;
	bool has_rl=false;

	while (q != e)
	{
		if (q->second.rl)
			has_rl=true;
		else
			has_lr=true;

		*p++ = *q;

		while (++q != e && q[-1].second == q->second)
			;
	}
	meta.erase(p, e);

	if (has_rl)
	{
		cached_dir=has_lr ? richtext_dir::both : richtext_dir::rl;
	}
}

richtext_dir richtextstring::get_dir(size_t from,
				     size_t to) const
{
	auto dir=richtext_dir::lr;

	if (from >= to)
		return dir;

	auto iter_from=meta_upper_bound_by_pos(meta, from);
	auto iter_to=meta_upper_bound_by_pos(meta, --to);

	if (iter_from == meta.begin() || iter_to == meta.begin())
		throw EXCEPTION("Internal error: iterator not found in meta_at()");
	bool has_lr=false;
	bool has_rl=false;

	--iter_from;

	while (iter_from < iter_to)
	{
		if (iter_from->second.rl)
			has_rl=true;
		else
			has_lr=true;

		++iter_from;
	}

	if (has_rl)
	{
		dir=has_lr ? richtext_dir::both : richtext_dir::rl;
	}

	return dir;
}

namespace {
#if 0
}
#endif

class compare_meta_by_pos {
public:

	inline bool operator()(const richtextstring::meta_t::value_type &v,
			       size_t p) const
	{
		return v.first < p;
	}

	inline bool operator()(size_t p,
			       const richtextstring::meta_t::value_type &v)
		const
	{
		return p < v.first;
	}
};
#if 0
{
#endif
}

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

richtextstring::meta_t::const_iterator
richtextstring::meta_upper_bound_by_pos(const meta_t &meta, size_t p)
{
	return std::upper_bound(meta.begin(), meta.end(), p,
				compare_meta_by_pos());
}

richtextmeta richtextstring::meta_at(size_t p)
{
	auto iter=meta_upper_bound_by_pos(meta, p);

	if (iter == meta.begin())
		throw EXCEPTION("Internal error: iterator not found in meta_at()");

	return (--iter)->second;
}

richtextstring::meta_t::iterator richtextstring::duplicate(size_t pos)
{
	assert_or_throw(pos <= string.size(),
			"Internal error: metadata is empty");

	if (pos == string.size())
		return meta.end();

	coalesce_needed=true;
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

void richtextstring::replace(size_t pos,
			     const richtextstring &other_string,
			     size_t other_pos, size_t other_size)
{
	size_t s=size();
	size_t other_s=other_string.size();

	assert_or_throw(pos <= s &&
			other_pos <= other_s &&
			other_s-other_pos >= other_size &&
			s-pos >= other_size,
			"Internal error: overflow in richtextstring::replace()"
			);

	if (other_size == 0)
		return;

	other_string.meta.reserve(other_string.meta.size()+2);
	auto other_meta_begin=duplicate(other_string.meta, other_pos);
	auto other_meta_end=duplicate(other_string.meta, other_pos+other_size);

	// Duplicate meta at insert begin and insert end points.
	//
	// reserve() ensures that duplicate() will not reallocate.

	meta.reserve(meta.size()+2+(other_meta_end-other_meta_begin));

	auto meta_begin=duplicate(pos);
	auto meta_end=duplicate(pos+other_size);

	// Erase the existing string's meta, and then insert the other_string
	// meta.
	meta.erase(meta_begin, meta_end);
	meta.insert(meta_begin, other_meta_begin, other_meta_end);

	// Update the starting position of the just-inserted meta.
	s=(other_meta_end-other_meta_begin);
	while (s)
	{
		meta_begin->first -= other_pos;
		meta_begin->first += pos;
		++meta_begin;
		--s;
	}

	//! And replace the contents of the string itself.
	auto ob=other_string.string.begin();

	std::copy(ob+other_pos, ob+other_pos+other_size,
		  string.begin()+pos);

	coalesce_needed=true;
}

void richtextstring::erase(size_t pos, size_t n)
{
	assert_or_throw(pos <= string.size() && string.size()-pos >= n,
			"Internal error: overflow in richtextstring::erase()");

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

void richtextstring::theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
{
	// This is mostly to clear the cached resolved fonts:
	modified();
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

	size_t n=string.size();
	string += o.string;

	size_t p=meta.size();

	meta.insert(meta.end(), o.meta.begin(), o.meta.end());

	for (auto b=meta.begin()+p, e=meta.end(); b != e; ++b)
		b->first += n;
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

//! Prepare string for to_canonical_order

//! As part of prepping, identify hotspot ranges and insert a newline before
//! the first character in the hotspot and after the last character in the
//! hotspot, and set start_end_hotspots to the indexes of the hotspots.
//!
//! The newlines get inserted directly into the richtextstring (and the
//! metadata gets updated accordingly.

struct richtextstring::to_canonical_order::prepped_string {

	richtextstring &string;

	// Locations of inserted newlines

	std::vector<size_t> start_end_hotspots;

	prepped_string(richtextstring &string,
		       bool replacing_hotspots);
};

richtextstring::to_canonical_order::prepped_string
::prepped_string(richtextstring &string,
		 bool replacing_hotspots)
	: string{string}
{
#ifdef TO_CANONICAL_ORDER_HOOK
	TO_CANONICAL_ORDER_HOOK();
#endif

	// Scan richtextstring metadata, noting the hotspot ranges

	text_hotspotptr current_hotspot{};

	for (auto &m:string.meta)
	{
		if (m.second.rl)
			throw EXCEPTION("internal error: canonicalized string "
					"passed to to_canonical_order");

		// Change in hotspot. This is the end of the current_hotspot
		// and/or start of a new hotspot.
		//
		// Append m.first to start_end_hotspots, noting where the
		// newline should go.
		//
		// Note that if we switched from one hotspot to another one
		// this will append m.first twice. There will be two newlines
		// here.
		if (m.second.link != current_hotspot)
		{
			if (m.first > 0 && current_hotspot)
				start_end_hotspots.push_back(m.first);

			if (m.second.link)
			{
				start_end_hotspots.push_back(m.first);
			}
		}
		current_hotspot=m.second.link;
	}

	// If we ended in a hotspot we'll add an additional newline to
	// terminate the last hotspot.
	if (current_hotspot)
		start_end_hotspots.push_back(string.size());

	// If we are replacing the contents of the hotspots, the replacement
	// text goes inside the existing hotspot markers so we don't need
	// to insert them here.
	if (replacing_hotspots)
		start_end_hotspots.clear();

	assert_or_throw((start_end_hotspots.size() % 2) == 0,
			"internal error: odd number of hotspot markers");

	// Now update the richtextstring. First we need to insert all the
	// newlines.
	//
	// We're going to resize() the richtextstring's string, making it
	// bigger by the number of newlines we're inserting. Then we're
	// going to work our way from the tail end of richtextstring.string
	// moving each character forward, and inserting the newlines where
	// they shuld go. So we we had (t: text, h:hotspot)
	//
	// string:   tttthhhhhhtttt
	//
	// start_end_hotspots: {4, 10}.
	//
	// we resize and end two more characters
	//
	// string:   tttthhhhhhtttt00
	//
	// and then work our way shifting it:
	//
	// string:   tttt<hhhhhh<tttt
	//
	// (< - newline )

	size_t s=string.size();

	size_t i=s;
	size_t j=s+start_end_hotspots.size();

	string.string.resize(j);

	size_t k=start_end_hotspots.size();

	if (k && start_end_hotspots[k-1] == s)
	{
		// Edge case, at the end of the string there's a newline.
		string.string.at(--j)='\n';
		--k;
	}

	// Move over, one character at a time. When i and j are equal
	// it means that all newlines have been inserted, and we can
	// stop now.

	while (i != j)
	{
		string.string.at(--j)=string.string.at(--i);

		// Do any newlines go here?
		while (k && i == start_end_hotspots[k-1])
		{
			string.string.at(--j)='\n';
			--k;
		}
	}

	// Now we need to update the metadata. We sweep the metadata again
	// from the beginning to the end. Each time we see the start of a
	// new hotspot it means that all subsequent metadata will start
	// two indices later, since we've inserted two newlines above.
	//
	// We just need to keep track of how much to adjust the start of
	// each metadata by, and we begin with 0.
	size_t incr=0;

	current_hotspot = text_hotspotptr{};

	for (auto &m:string.meta)
	{
		m.first += incr;

		if (m.second.link != current_hotspot)
		{
			// New metadata started? Everything else that
			// follows is shifted over by two.
			if (m.second.link)
				incr += 2;
		}

		current_hotspot=m.second.link;
	}

	// We now update start_end_hotspots to reflect the location of the
	// newlines in the richtextstring.string, where the newlines have
	// been inserted.
	//
	// start_end_hotspots started of by indicates the indices where
	// the newlines need to get inserted, and they'll now indicate
	// the index of each inserted newline.

	incr=0;

	for (i=0, j=start_end_hotspots.size(); i < j; i += 2)
	{
		start_end_hotspots[i] += incr;
		start_end_hotspots[i+1] += incr+1;
		incr += 2;
	}
}

richtextstring::to_canonical_order
::to_canonical_order(richtextstring &string,
		     const std::optional<unicode_bidi_level_t>
		     &paragraph_embedding_level,
		     bool replacing_hotspots)
	: to_canonical_order{prepped_string{string, replacing_hotspots},
	paragraph_embedding_level}
{
}

static unicode_bidi_level_t default_paragraph_embedding_level()
{
#ifdef TEST_DEFAULT_EMBEDDING_LEVEL

	return TEST_DEFAULT_EMBEDDING_LEVEL;
#else
	static const bool right_to_left=
		locale::base::environment()->right_to_left();

	return right_to_left ? UNICODE_BIDI_RL : UNICODE_BIDI_LR;
#endif
}

richtextstring::to_canonical_order
::to_canonical_order(prepped_string &&the_prepped_string,
		     const std::optional<unicode_bidi_level_t>
		     &paragraph_embedding_level)
	: string{the_prepped_string.string},
	  types{string.string},
	  calc_results{unicode::bidi_calc(types,
					  paragraph_embedding_level
					  ? *paragraph_embedding_level
					  : default_paragraph_embedding_level()
					  )},
	  starting_pos{0},
	  n_chars{0},
	  ending_pos{0}
{
	types.setbnl(string.string);

	auto &direction = std::get<1>(calc_results);

	// If there's no explicit paragraph embedded was determined we will
	// use one from the environment. This will result in the default
	// paragraph embedding level for new, empty input field to be taken
	// from the environment locale.

	if (!direction.is_explicit)
	{
		direction.direction=default_paragraph_embedding_level();
	}

#ifdef TO_CANONICAL_ORDER_HOOK
	TO_CANONICAL_ORDER_HOOK();
#endif

	// Update the richtextstring and replace all newlines that were
	// inserted as demarcation marks for the hotspot, and replace them
	// with the hotspot_marker, and update their type.

	for (auto i:the_prepped_string.start_end_hotspots)
	{
		string.string.at(i) = hotspot_marker;
		types.types.at(i) = UNICODE_BIDI_TYPE_S;
	}
}

struct richtextstring::meta_reverse {

	richtextstring &string;
	const size_t starting_pos;
	const size_t s;

	meta_reverse(richtextstring &string, size_t starting_pos)
		: string{string}, starting_pos{starting_pos},
		  s{string.size()}
	{
	}

	void operator()(size_t start, size_t n)
	{
		// bidi_reorder passes start relative to starting_pos

		start += starting_pos;

		assert_or_throw(start <= s && (s-start) >= n,
				"invalid reordering callback "
				"parameters");

		if (n == 0)
			return;

		// Find the start+ending metadata range.

		string.meta.reserve(string.meta.size()+2);

		auto from=string.duplicate(start);
		auto to=string.duplicate(start+n);

		// We will rebuild the swapped metadata in meta_cpy.
		meta_t meta_cpy;

		meta_cpy.reserve(to-from);

		// Iterate from the end to the beginning.

		// The ending position is equivalent to the
		// starting position:
		auto current_pos=start;

		// As we iterate backwards we compute how many
		// characters have the metadata applied to them.
		auto prev_end_pos=start+n;

		while (from < to)
		{
			--to;

			meta_cpy.emplace_back(current_pos, to->second);

			// (prev_end_pos-to) is the number of
			// characters that this metadata applies to.
			current_pos += prev_end_pos-to->first;

			prev_end_pos=to->first;
		}

		// We have a 1:1 relationship, so just update in place.
		for (auto &n:meta_cpy)
			*from++=n;
	};
};

void richtextstring::to_canonical_order::fill()
{
	if (filled)
		return;

	// Find the next paragraph break
	auto tb=types.types.begin()+starting_pos;
	auto te=types.types.end();

	auto tp=std::find(tb, te, UNICODE_BIDI_TYPE_B);

	if (tp != te)
		++tp;

	// Set the number of characters in this paragraph, and
	// compute the resulting ending_pos (next paragraph's starting_pos
	// if it gets to this point)

	n_chars=tp-tb;
	ending_pos=starting_pos+n_chars;

	// Reorder just the corresponding part of the original richtextstring
	//
	// We'll swap the metadata ourselves.

	auto &levels=std::get<0>(calc_results);

	unicode::bidi_reorder
		(string.string, levels,
		 meta_reverse{string, starting_pos},
		 starting_pos,
		 ending_pos-starting_pos);

	// Now we remove the bidirectional markers to get the
	// CANONICAL_CLEANUP-ed string.

	std::vector<size_t> removed_indices;

	unicode::bidi_cleanup
		(string.string, levels,
		 [&, this]
		 (size_t pos)
		 {
			 // Relative to the starting position.
			 removed_indices.push_back(pos+starting_pos);

			 // We have one fewer character now. This keeps track
			 // of the actual number of characters in this
			 // paragraph.
			 --n_chars;
		 },
		 unicode::literals::CLEANUP_CANONICAL,
		 starting_pos,
		 ending_pos-starting_pos);

	if (removed_indices.size())
	{
		// Now, for each index we'll need to dupe that and the
		// following index. We're removing this character from the
		// string, and we need to update the metadata. This makes
		// sure that there is an entry for the removed character and
		// the following character, in the metadata vector, and we
		// remove the removed character's metadata, below.

		string.meta.reserve(string.meta.size() +
				    removed_indices.size()*2);

		for (auto i:removed_indices)
		{
			string.duplicate(i);

			if (i+1 < string.size())
				string.duplicate(i+1);
		}

		// So, we need to adjust the remaining meta range, by
		// doing two things:

		auto mb=string.meta_lower_bound_by_pos(removed_indices[0]);
		auto p=mb;
		auto me=string.meta.end();

		auto rb=removed_indices.begin(), re=removed_indices.end();

		size_t adjust=0;

		while (mb != me)
		{
			if (rb != re && mb->first == *rb)
			{
				// 1) Skipping over removed character indexes.

				// Each skipped-over character means that the
				// metadata starting position of all following
				// metadata entries gets decremented by 1.

				++adjust;
				++rb;
			}
			else
			{
				// 2) And adjusting the remaining indexes;

				*p=*mb;
				p->first -= adjust;
				++p;
			}
			++mb;
		}

		string.meta.erase(p, me);
	}

	// And now, set the rl flag.
	//
	// We sweep through the levels, and update the metedata accordingly.
	assert_or_throw(starting_pos <= levels.size() &&
			levels.size()-starting_pos >= n_chars,
			"internal error: unexpected levels size");

	auto bb=levels.begin();
	auto b=bb+starting_pos;
	auto e=b+n_chars;

	// The paragraph break, \n, force it to its embedding level.

	if (b < e)
	{
		// The paragraph break will be either the last or the
		// first character.
		size_t i=starting_pos;

		if (string.string.at(starting_pos+n_chars-1) == '\n')
			i=starting_pos+n_chars-1;

		if (string.string.at(i) == '\n')
			levels.at(i)=paragraph_embedding_level();
	}

	while (b != e)
	{
		// Find the start of the next sequence of rtol text.

		if (*b == UNICODE_BIDI_LR)
		{
			++b;
			continue;
		}

		auto p=b;

		// And now find the end of the rtol text.
		while (b != e)
		{
			if (*b == UNICODE_BIDI_LR)
				break;
			++b;
		}

		// We just need to set the rtol flag in the corresponding
		// metadata range.
		string.meta.reserve(string.meta.size()+2);

		auto q=string.duplicate(p-bb);
		auto to=string.duplicate(b-bb);

		while (q != to)
		{
			q->second.rl=true;
			++q;
		}
	}

	canonical_string=richtextstring{string, starting_pos, n_chars};

	filled=true;
}

richtextstring::from_canonical_order
::from_canonical_order(richtextstring &string,
		       unicode_bidi_level_t paragraph_embedding)
	: string{string}, paragraph_embedding{paragraph_embedding}
{
	// Recreate levels
	levels.reserve(string.string.size());

	size_t prev_index=0;
	unicode_bidi_level_t prev_lr=UNICODE_BIDI_LR;

	for (const auto &m:string.meta)
	{
		while (prev_index < m.first)
		{
			levels.push_back(prev_lr);
			++prev_index;
		}

		prev_lr=m.second.rl ? UNICODE_BIDI_RL:UNICODE_BIDI_LR;
	}

	size_t s=string.size();

	while (prev_index < s)
	{
		levels.push_back(prev_lr);
		++prev_index;
	}

	// We can now convert to logical order ...
	unicode::bidi_logical_order(string.string, levels,
				    paragraph_embedding,
				    meta_reverse{string, 0});

	// and then clear the rl flag.
	for (auto &m:string.meta)
		m.second.rl=false;
	string.coalesce_needed=true;
	string.shrink_to_fit();
}

richtextstring richtextstring::from_canonical_order::embed() const
{
	richtextstring new_string;

	// Estimate expansion.
	new_string.string.reserve(string.string.size() / 9 * 10);
	new_string.meta.reserve(string.meta.size() / 5 * 7);

	unicode::bidi_embed
		(string.string, levels,
		 paragraph_embedding,
		 [&, this]
		 (const char32_t *s,
		  size_t n,
		  bool is_part_of_string)
		 {
			 if (!is_part_of_string)
			 {
				 // Make sure there's meta here.

				 if (new_string.string.empty())
					 new_string.meta.push_back
						 (string.meta.at(0));

				 new_string.string.insert
					 (new_string.string.end(),
					  s, s+n);
				 return;
			 }

			 // We'll fill in the metadata starting at offset
			 // meta_p.
			 size_t meta_p=new_string.size();

			 // Copying the string, first.
			 new_string.string.insert(new_string.string.end(),
						  s, s+n);

			 // Now copy over the metadata. First, compute which
			 // is_part_of_string is s.

			 size_t off=s-&*string.string.begin();

			 auto iter=string.duplicate(off);

			 // Copy the metadata until end of string or until
			 // the metadata is on or after the end of the
			 // copied string.
			 auto e=string.meta.end();
			 while (iter != e)
			 {
				 if (iter->first >= off+n)
					 break;
				 auto &m=*iter++;

				 // Need to adjust the indices. Subtract the
				 // starting position in the old string, add
				 // the starting position in the new string.
				 new_string.meta
					 .emplace_back(m.first-off + meta_p,
						       m.second);
			 }
		 });

	new_string.coalesce_needed=true;
	string.shrink_to_fit();
	return new_string;
}

void richtextstring::from_canonical_order
::embed_paragraph(richtextstring &string,
		  unicode_bidi_level_t paragraph_embedding)
{
	if (char32_t c=unicode::bidi_embed_paragraph_level(string.string,
							   paragraph_embedding))
	{
		string.string.insert(string.string.begin(), c);
		for (auto &m:string.meta)
		{
			if (m.first > 0)
				++m.first;
		}
	}
}

LIBCXXW_NAMESPACE_END
