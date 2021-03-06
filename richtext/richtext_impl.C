/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_impl.H"
#include "richtext/richtext_range.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "richtext/richtext_insert.H"
#include "x/w/screen.H"
#include "messages.H"
#include "assert_or_throw.H"

#include <x/vector.H>
#include <x/sentry.H>
#include <courier-unicode.h>
#include <algorithm>

LIBCXXW_NAMESPACE_START

richtext_implObj::richtext_implObj(richtextstring &&string,
				   const richtext_options &options)
	: word_wrap_width{0},
	  is_editor{options.is_editor},
	  unprintable_char{options.unprintable_char},
	  requested_alignment{options.alignment},
	  requested_paragraph_embedding_level{
		  options.paragraph_embedding_level},
	  requested_directional_format{options.directional_format},
	  alignment{halign::left}, // To be updated, shortly
	  paragraph_embedding_level{UNICODE_BIDI_LR} // To be updated, shortly
{
	do_set(std::move(string));
}

fragment_cursorlocations_t richtext_implObj::set(ONLY IN_THREAD,
						 richtextstring &&string)
{
	// If the existing rich text object has any cursor locations, make
	// a copy of them.

	fragment_cursorlocations_t all_locations;

	paragraphs.for_paragraphs
		(0,
		 [&]
		 (const auto &paragraph)
		 {
			 paragraph->fragments.for_fragments
				 ([&]
				  (const auto &fragment)
				  {
					  all_locations.insert
						  (all_locations.end(),
						   fragment->locations.begin(),
						   fragment->locations.end());
				  });
			 return true;
		 });

	assert_or_throw(all_locations.empty() || !string.get_string().empty(),
			"Attempting to set empty string contents when there are existing locations");

	// Also restore the original paragraphs in the event of a thrown exception.

	auto paragraphs_cpy=paragraphs;
	auto alignment_cpy=alignment;
	auto paragraph_embedding_level_cpy=paragraph_embedding_level;

	auto restore_paragraphs_sentry=
		make_sentry([&, this]
			    {
				    paragraphs=paragraphs_cpy;
				    alignment=alignment_cpy;
				    paragraph_embedding_level=
					    paragraph_embedding_level_cpy;
			    });

	restore_paragraphs_sentry.guard();

	do_set(std::move(string));

	// Ok, now move all locations to the last character of the new text.

	if (!all_locations.empty())
	{
		assert_or_throw(!paragraphs.empty(),
				"Internal error: paragraphs are empty");
		auto last_paragraph=
			*paragraphs.get_paragraph(paragraphs.size()-1);

		assert_or_throw(!last_paragraph->fragments.empty(),
				"Internal error: last paragraph is empty");

		auto last_fragment=*last_paragraph->fragments
			.get_iter(last_paragraph->fragments.size()-1);

		assert_or_throw(!last_fragment->string.get_string().empty() &&
				last_fragment->locations.empty(),
				"Internal error: first fragment cannot be empty in set()");

		// If an unlikely exception gets thrown, clear the locations
		// array
		auto locations_sentry=make_sentry
			([&]
			 {
				 last_fragment->locations.clear();
			 });
		locations_sentry.guard();

		last_fragment->locations=all_locations;

		locations_sentry.unguard();

		// Position transplanted locations at the end of last fragment.

		auto fragment_ptr=&*last_fragment;
		size_t n=fragment_ptr->string.size()-1;

		for (auto b=fragment_ptr->locations.begin(),
			     e=fragment_ptr->locations.end(); b != e; ++b)
		{
			(*b)->initialize(fragment_ptr, n, b,
					 new_location::bidi);
		}
	}

	restore_paragraphs_sentry.unguard();

	return all_locations;
}

void richtext_implObj::do_set(richtextstring &&string)
{
	paragraphs.clear();
	num_chars=0;

	paragraph_list my_paragraphs(*this);

	// Begin the job of unpacking this stuff and set the final
	// paragraph embedding level
	create_fragments_from_inserted_text factory{string,
		requested_paragraph_embedding_level,
		false,
		paragraph_embedding_level};

	// Set the final alignment
	if (requested_alignment)
		alignment= *requested_alignment;
	else
		alignment= paragraph_embedding_level == UNICODE_BIDI_LR
			? halign::left : halign::right;

	while (1)
	{
		auto string=factory.next_string();

		if (string.size() == 0)
			break;

		auto new_paragraph=
			my_paragraphs.append_new_paragraph();

		fragment_list my_fragments{my_paragraphs,
			*new_paragraph};

		my_fragments.append_new_fragment(std::move(string));
	}
}

richtext_implObj::~richtext_implObj()=default;

void richtext_implObj::rich_text_paragraph_out_of_bounds()
{
	throw EXCEPTION("Internal error: rich text paragraph out of bounds.");
}

bool richtext_implObj::rewrap(dim_t width)
{
	if (word_wrap_width == width)
		// richtextObj's constructor calls shrink_to_fit() upon
		// a false return.
		return false;

	word_wrap_width=width;

	if (word_wrap_width == 0)
		return unwrap();

	paragraph_list my_paragraphs{*this};

	auto flag=my_paragraphs.rewrap(word_wrap_width);
	my_paragraphs.shrink_to_fit();
	return flag;
}

bool richtext_implObj::unwrap()
{
	paragraph_list my_paragraphs(*this);

	auto flag=my_paragraphs.unwrap();

	my_paragraphs.shrink_to_fit();

	return flag;
}

size_t richtext_implObj::find_paragraph_for_pos(size_t &pos)
{
	auto p=paragraphs.find_paragraph_for_pos(pos);

	pos -= p->first_char_n;

	if (pos >= p->num_chars) // Last paragraph
	{
		assert_or_throw(p->num_chars > 0,
				"Internal error: empty paragraph in find_paragraph_for_pos()");
		pos=p->num_chars-1;
	}

	return p->my_paragraph_number;
}

richtextparagraph richtext_implObj::paragraphs_t::find_paragraph_for_pos(size_t n)
{
	auto iter=std::lower_bound(begin(), end(), n,
				   []
				   (const richtextparagraph &p, size_t n)
				   {
					   return p->first_char_n <= n;
				   });

	assert_or_throw(iter != begin(),
			"Internal error: empty text in find_paragraph_for_pos");

	return *--iter;
}

richtextfragmentptr richtext_implObj::find_fragment_for_y_position(size_t y_pos)
{
	return paragraphs.find_fragment_for_y_position(y_pos);
}

richtextfragmentptr
richtext_implObj::paragraphs_t::find_fragment_for_y_position(size_t y_pos)
{
	richtextfragmentptr ret;

	auto iter=std::lower_bound(begin(), end(), y_pos,
				   []
				   (const richtextparagraph &p, size_t y_pos)
				   {
					   return p->first_fragment_y_position
					       <= y_pos;
				   });

	if (iter != begin())
	{
		auto paragraph=*--iter;

		y_pos -= paragraph->first_fragment_y_position;

		ret=paragraph->fragments.find_fragment_for_y_position(y_pos);
	}
	return ret;
}

richtextfragmentptr
richtextfragmentObj::fragments_t::find_fragment_for_y_position(size_t y_pos)
{
	richtextfragmentptr ret;

	auto iter=std::lower_bound(begin(), end(), y_pos,
				   []
				   (const richtextfragment &p, size_t y_pos)
				   {
					   return p->y_pos <= y_pos;
				   });

	// Note that the comparison is p->y_pos <= y_pos. That means that if
	// the fragment's p_ypos is y_pos, the lower_bound() returns the
	// iterator to the *next* fragment.

	if (iter != begin())
	{
		auto f=*--iter;

		// Return the last fragment unless y_pos is below the last
		// fragment.

		y_pos -= f->y_pos;

		if (y_pos < dim_t::value_type(f->height()))
			ret=f;
	}
	return ret;
}

richtextstring richtext_implObj::get_as_richtext() const
{
	// Size up our job, first.

	size_t total_chars=0;
	size_t total_meta=0;

	paragraphs.for_paragraphs
		(0,
		 [&]
		 (const auto &paragraph)
		 {
			 paragraph->fragments.for_fragments
				 ([&]
				  (const auto &fragment)
				  {
					  total_chars=fragment->string
						  .size();
					  total_meta=fragment->string
						  .get_meta().size();
				  });
			 return true;
		 });

	richtextstring s;

	s.reserve(total_chars, total_meta);

	paragraphs.for_paragraphs
		(0,
		 [&]
		 (const auto &paragraph)
		 {
			 paragraph->fragments.for_fragments
				 ([&]
				  (const auto &fragment)
				  {
					  s += fragment->string;
				  });
			 return true;
		 });

	return s;
}

void richtext_implObj::theme_updated(ONLY IN_THREAD,
					 const const_defaulttheme &new_theme)
{
	paragraph_list my_paragraphs(*this);

	my_paragraphs.theme_updated(IN_THREAD, new_theme);
}


void richtext_implObj::rewrap_at_fragment(dim_t width,
					  richtextfragmentObj *fragment,
					  fragment_list &fragment_list_arg,
					  richtext_insert_results
					  &insert_results)
{
	assert_or_throw(fragment && fragment->my_paragraph,
			"Internal error: fragment or paragraph is null");

	int n=1;
	auto paragraph=fragment->my_paragraph;
	auto fragment_n=fragment->my_fragment_number;
	if (fragment_n)
	{
		--fragment_n;
		++n;
	}

	paragraph_list my_paragraphs{*this};
	fragment_list my_fragments{my_paragraphs, *paragraph};

	bool wrapped=false;
	while (n && fragment_n < paragraph->fragments.size())
	{
		bool toosmall, toobig;

		paragraph->rewrap_fragment(my_fragments,
					   width,
					   fragment_n,
					   toosmall, toobig,
					   insert_results);
		if (toobig || toosmall)
			wrapped=true;
		if (toobig)
			++n;

		if (!toosmall && !toobig)
			--n;
		++fragment_n;
	}

	if (wrapped)
		my_fragments.fragments_were_rewrapped();
}

void richtext_implObj::set_updated(ONLY IN_THREAD, richtextObj &public_object,
				   const richtext_insert_base &new_text)
{
	//! Find the trailing newline

	auto last=public_object.end();

	auto f=last->my_location->my_fragment;
	auto o=last->my_location->get_offset();

	assert_or_throw(f,
			"Internal error: null my_fragment in insert()");

	auto space=f->string.get_string().at(o);
	auto meta=f->string.meta_at(o);

	assert_or_throw(space == '\n',
			"internal error: didn't find trailing newline");
	meta.rl=false;

	auto new_string=new_text(meta);

	new_string += richtextstring{
		std::u32string{&space, &space+1},
		{
			{0, meta},
				}};

	auto cursors_to_move=set(IN_THREAD, std::move(new_string));

	auto b=public_object.begin();
	auto e=public_object.end();

	richtextfragment top_fragment{b->my_location->my_fragment};
	auto top_from=b->my_location->get_offset();
	richtextfragment bottom_fragment{e->my_location->my_fragment};
	auto bottom_to=e->my_location->get_offset();

	for (const auto &l:cursors_to_move)
	{
		if (l->do_not_adjust_in_insert)
		{
			l->reposition(top_fragment, top_from);
		}
		else
		{
			l->reposition(bottom_fragment, bottom_to);
		}
	}
}

void richtext_implObj::insert_at_location(ONLY IN_THREAD,
					  richtextObj &public_object,
					  const richtext_insert_base
					  &new_text)
{
	if (is_editor && num_chars == 1)
	{
		// We must be an editor, which has an extra trailing newline
		// where the cursor rests after all existing input.
		set_updated(IN_THREAD, public_object, new_text);
		return;
	}
	paragraph_list my_paragraphs{*this};

	insert_at_location(IN_THREAD, my_paragraphs, new_text);
}

void richtext_implObj::insert_at_location(ONLY IN_THREAD,
					  paragraph_list &my_paragraphs,
					  const richtext_insert_base
					  &new_text)
{
	assert_or_throw(new_text.fragment(),
			"Internal error: null my_fragment in insert()");

	// Start by inserting the text. insert() returns the number of
	// inserted paragraph breaks.

	auto orig_fragment=new_text.fragment();
	auto inserted_info=orig_fragment->insert(IN_THREAD,
						 my_paragraphs,
						 new_text);

	if (word_wrap_width > 0)
	{
		assert_or_throw(orig_fragment && orig_fragment->my_paragraph &&
				orig_fragment->my_paragraph->my_richtext,
				"my_fragment, my_paragraph, or my_richtext "
				"is null in "
				"lock_and_insert_at_location()");

		// First, rewrap whole paragraphs inserted.
		//
		// If there's more than one fragment that was inserted, we
		// must've inserted new paragraphs, rewrap them.
		auto counter=inserted_info.inserted_range.size();

		if (inserted_info.insert_ended_in_paragraph_break)
			++counter;

		if (counter > 1)
		{
			paragraphs.for_paragraphs
				(orig_fragment->my_paragraph
				 ->my_paragraph_number+1,
				 counter-1,
				 [&]
				 (const richtextparagraph &p)
				 {
					 p->rewrap(my_paragraphs,
						   word_wrap_width,
						   inserted_info);
					 return true;
				 });
		}

		// Now, rewrap the inserted-into paragraph

		fragment_list my_fragments{my_paragraphs,
				*orig_fragment->my_paragraph};

		rewrap_at_fragment(word_wrap_width,
				   orig_fragment, my_fragments,
				   inserted_info);
	}

	inserted_info.move_cursors();
}

//! Calculate which portion of the line fragment falls in the removal range.

//! Subclasses richtext_range to determine which portions of a given fragment
//! are within the removal range.

struct richtext_implObj::remove_info : richtext_range {

	using richtext_range::richtext_range;

	//! Which part of the given fragment is in the selection range.

	//! If all or part of the given fragment is in the selected richtext
	//! range, return the first character index and the number of
	//! characters.
	//!
	//! Number of characters being 0 indicates no part of this fragment
	//! is in range.

	std::tuple<size_t, size_t> in_range(const richtextfragmentObj *f)
	{
		auto index=f->index();

		// Edge case, before or after the range:

		auto starting_index=location_a->my_fragment->index();
		auto ending_index=location_b->my_fragment->index();

		if (index < starting_index || index > ending_index)
			return {0, 0}; // Shouldn't happen.

		// If this is the first or the last fragment in the range,
		// use range_embedding_level() then the appropriate
		// richtext_range method, and capture what comes out of
		// range().

		if (index == starting_index || index == ending_index)
		{
			first=0;
			last=0;

			if (complete_line())
			{
				// ... but this is all on one line.

				return {first, last-first};
			}

			auto embedding_level=f->range_embedding_level();

			if (embedding_level == UNICODE_BIDI_LR)
			{
				lr_lines(nullptr, f, f);
			}
			else
			{
				rl_lines(f, f, nullptr);
			}

			return {first, last-first};
		}

		// Completely inside the range. Shouldn't happen, we get
		// called only for the starting and ending character range.

		return {0, f->string.size()};
	}
private:

	mutable size_t first=0, last=0;

	// Capture what lr_lines() or rl_lines() feeds us, piecemeal, and
	// assume that what's in range is the combined part.

	void range(const richtextstring &other,
		   size_t start,
		   size_t n) const override
	{
		if (n == 0)
			return;

		if (first == last)
			first=last=start;

		if (start < first)
			first=start;

		start += n;

		if (start > last)
		{
			last=start;
		}
	}

};

void richtext_implObj::remove_at_location(const richtextcursorlocation &ar,
					  const richtextcursorlocation &br)
{
	remove_info info{paragraph_embedding_level, ar, br};

	if (info.diff == 0)
		return; // Too easy

	paragraph_list my_paragraphs{*this};

	remove_at_location(info, my_paragraphs);
}

void richtext_implObj
::replace_at_location(ONLY IN_THREAD,
		      richtextObj &public_object,
		      const richtext_insert_base &new_text,
		      const richtextcursorlocation &remove_from,
		      const richtextcursorlocation &remove_to)
{
	if (is_editor)
	{
		// This is replacing entire contents if one of the locations
		// is 0, and the other is one less than num_chars.
		auto pos1=remove_from->pos(get_location::bidi);
		auto pos2=remove_to->pos(get_location::bidi);

		if ((pos1 == 0 || pos2 == 0) && pos1+pos2+1 == num_chars)
		{
			set_updated(IN_THREAD, public_object, new_text);
			return;
		}
	}

	remove_info info{paragraph_embedding_level, remove_from, remove_to};

	paragraph_list my_paragraphs{*this};

	if (info.diff != 0)
	{
		richtext_insert_results ignored;

		remove_at_location_no_rewrap(info,
					     my_paragraphs,
					     ignored);
	}

	insert_at_location(IN_THREAD, my_paragraphs, new_text);
}

std::pair<metrics::axis, metrics::axis>
richtext_implObj::get_metrics(dim_t preferred_width)
{
	dim_t w= dim_t::truncate(width());
	dim_t h= dim_t::truncate(height());

	if (w >= dim_t::infinite())
		w=w-1;
	if (h >= dim_t::infinite())
		h=h-1;

	auto min_width=w;
	auto max_width=w;

	if (word_wrap_width > 0)
	{
		// This label is word-wrapped, and it is visible.
		// We compute the metrics like this. Here's our minimum
		// and maximum widths:
		max_width=dim_t::truncate(real_maximum_width);

		if (max_width == dim_t::infinite()) // Let's not go there.
			max_width=max_width-1;

		min_width=minimum_width;

		// And let's try to be sane.

		if (min_width > max_width)
			min_width=max_width;

		w=preferred_width;

		if (w < min_width)
			w=min_width;

		if (w > max_width)
			w=max_width;
	}

	return {
		{min_width, w, max_width},
		{h, h, h}
	};
}

void richtext_implObj::remove_at_location(remove_info &info,
					  paragraph_list &my_paragraphs)
{
	richtext_insert_results ignored;

	remove_at_location_no_rewrap(info, my_paragraphs, ignored);

	if (word_wrap_width > 0)
	{
		// Note: the text removal inside remove_at_location_no_rewrap
		// destroys the fragment_a_list which updates fragment sizes.
		//
		// This is needed before invoking rewrap_at_fragment, which
		// then looks at the current fragment sizes, in order to decide
		// what to rewrap.
		//
		// We need to update the fragment's minimum_size, in order
		// to consider whether the fragment can be wrapped back to its
		// previous fragment, reflecting the removed text.

		auto fragment_a=info.location_a->my_fragment;

		fragment_list fragment_a_list(my_paragraphs,
					      *fragment_a->my_paragraph);
		rewrap_at_fragment(word_wrap_width,
				   fragment_a, fragment_a_list,
				   ignored);
	}
}

void richtext_implObj
::remove_at_location_no_rewrap(remove_info &info,
			       paragraph_list &my_paragraphs,
			       richtext_insert_results &results)
{
	assert_or_throw(info.location_a->my_fragment &&
			info.location_b->my_fragment,
			"uninitialized fragments in remove_between()");

	auto fragment_a=info.location_a->my_fragment;
	auto fragment_b=info.location_b->my_fragment;

	auto diff=info.diff;

	auto [a_start, a_size]=info.in_range(fragment_a);
	auto [b_start, b_size]=info.in_range(fragment_b);

	if (diff <= 1) // Same line
	{
		fragment_list fragment_a_list{my_paragraphs,
			*fragment_a->my_paragraph};

		fragment_a->remove(a_start,
				   a_size,
				   fragment_a_list,
				   results);

		auto embedding_level = fragment_a->embedding_level();

		if (embedding_level == UNICODE_BIDI_LR ?
		    fragment_a->string.size() == a_start :
		    a_start == 0)
		{
			if (fragment_a->next_fragment())
			{
				fragment_a->merge(fragment_a_list,
						  fragment_a->merge_bidi,
						  results)
					->recalculate_linebreaks();
			}
			else
				fragment_a->recalculate_linebreaks();

		} else if (embedding_level == UNICODE_BIDI_LR ?
			   a_start == 0 :
			   fragment_a->string.size() == a_start)
		{
			if (fragment_a->my_fragment_number)
			{
				fragment_a->prev_fragment()
					->merge(fragment_a_list,
						fragment_a->merge_bidi,
						results)
					->recalculate_linebreaks();
			}
			else
				fragment_a->recalculate_linebreaks();
		}
		return;
	}

	fragment_list fragment_a_list(my_paragraphs,
				      *fragment_a->my_paragraph);

	assert_or_throw(a_size < fragment_a->string.size() ||
			b_size < fragment_b->string.size(),
			"Internal error: cannot be removing both the starting "
			"and the ending line completely");

	if (diff > 1)
	{
		// Remove intermediate fragments completely, and quickly.

		--diff;
		auto p=fragment_a->next_fragment();

		if (--diff)
		{
			while (diff)
			{
				results.removed(ref{p});
				{
					fragment_list
						rem_fragments{my_paragraphs,
							      *p->my_paragraph};

					diff -= rem_fragments
						.remove(p->my_fragment_number,
							diff,
							fragment_b);
				}

				p=fragment_a->next_fragment();
			}
		}

		// Then merge the next fragment into this one, according
		// to the paragraph embedding level.
		//
		// The merge will reposition locations "a" and "b". We want
		// to remember where [a_start, a_size] and [b_start, b_size]
		// is relative to the location of the fragments.
		ssize_t relative_a_start=a_start-info.location_a->get_offset();
		ssize_t relative_b_start=b_start-info.location_b->get_offset();
		fragment_a->merge(fragment_a_list,
				  fragment_a->merge_paragraph,
				  results);
		my_paragraphs.recalculation_required();

		auto f=info.location_a->my_fragment;

		assert_or_throw(f == info.location_b->my_fragment,
				"Internal error: expected to merge two "
				"fragments into one");

		// We now reapply relative_a_start and relative_b_start
		// to locations "a" and "b" in order to know where those
		// are now.

		auto new_a_offset=info.location_a->get_offset();
		auto new_b_offset=info.location_b->get_offset();

		// Do some sanity checks, first.

		assert_or_throw( (relative_a_start < 0 ?
				  -(size_t)relative_a_start <= new_a_offset :
				  f->string.size()-new_a_offset >=
				  (size_t)relative_a_start) &&
				 (relative_b_start < 0 ?
				  -(size_t)relative_b_start <= new_b_offset :
				  f->string.size()-new_b_offset >=
				  (size_t)relative_b_start),
				 "Internal error: inconsistent ranges after "
				 "fragment merge");

		a_start=new_a_offset + relative_a_start;
		b_start=new_b_offset + relative_b_start;

		// Make sure "a" range is before the "b" range, then figure
		// out what we have.

		if (b_start < a_start)
		{
			std::swap(a_start, b_start);
			std::swap(a_size, b_size);
		}

		// We don't expect b to be wholly inside a, but if we do:
		if (b_start+b_size <= a_start+a_size)
		{
			b_start=a_start+a_size;
			b_size=0;
		}

		// We don't expect b to start in the middle of a, either...

		if (b_start < a_start+a_size)
		{
			size_t adjust=a_start+a_size-b_start;

			b_start += adjust;
			b_size -= adjust;
		}

		f->remove(b_start, b_size, fragment_a_list, results);
		f->remove(a_start, a_size, fragment_a_list, results);
		return;
	}

	auto a=info.location_a;
	auto b=info.location_b;

	if (a->compare(*b) > 0)
		std::swap(a, b);

	a->my_fragment->remove(a->get_offset() +
			       // This is the *ending* location. and 'b' is the
			       // *starting* location.
			       (fragment_a->embedding_level() == UNICODE_BIDI_LR
				? 0:1),
			       a_size + b_size,
			       fragment_a_list,
			       results);
}

LIBCXXW_NAMESPACE_END
