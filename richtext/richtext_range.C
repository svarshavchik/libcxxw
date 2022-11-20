/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_range.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtextfragment.H"
#include "x/w/impl/richtext/richtext.H"
#include "x/w/text_param.H"
#include "x/w/richtext/richtextiterator.H"
#include "assert_or_throw.H"

LIBCXXW_NAMESPACE_START

richtext_range::richtext_range(unicode_bidi_level_t paragraph_embedding_level,
			       const richtextcursorlocation &a,
			       const richtextcursorlocation &b)
	: paragraph_embedding_level{paragraph_embedding_level},
	  location_a{a},
	  location_b{b},
	  diff{location_a->compare(*location_b)}
{
	if (diff == 0)
		return;

	// Make sure we go from a to b.

	if (diff > 0)
	{
		std::swap(location_a, location_b);
	}
	else
	{
		diff= -diff;
	}

	assert_or_throw(location_a->my_fragment &&
			location_b->my_fragment,
			"Internal error: uninitialized fragments"
			" in get()");

	location_a_offset=location_a->get_offset();
	location_b_offset=location_b->get_offset();
}

bool richtext_range::complete_line()
{
	if (diff == 0)
		return true; // Too easy

	if (diff != 1)
		return false;

	// On the same line.

	if (location_a->my_fragment->embedding_level() == UNICODE_BIDI_LR)
	{
		// Left to right text, get() characters starting
		// with the starting position, location_a, until
		// (but not including) location_b.
		add(location_a->my_fragment->string,
		    location_a->get_offset(),
		    location_b->get_offset()-
		    location_a->get_offset());
	}
	else
	{
		// Right to left text. get() characters starting
		// (but not including) the starting position,
		// location_a, until and including location b.

		add(location_a->my_fragment->string,
		    location_a->get_offset()+1,
		    location_b->get_offset()-
		    location_a->get_offset());
	}
	return true;
}

// Use the unicode wordbreaking algorithm to find words.

namespace {
#if 0
}
#endif

// Capture word-breaking flags out of courier-unicode

struct find_word : unicode::wordbreak_callback_base {

	using unicode::wordbreak_callback_base::operator<<;

	size_t pos;

	size_t begin_pos;
	size_t end_pos;

	find_word(size_t pos) : pos{pos}, begin_pos{0}, end_pos{0}
	{
	}

	size_t i{0};
	bool found_after_end{false};

	int callback(bool flag) override
	{
		if (flag)
		{
			// Word break before this character.

			// We capture the last word break before pos

			if (i <= pos)
				begin_pos=i;

			// Now we capture the first word break on or after pos

			if (i >= pos && i > begin_pos)
			{
				if (!found_after_end)
					end_pos=i;
				found_after_end=true;
			}
		}
		++i;
		return 0;
	}

	void finish()
	{
		unicode::wordbreak_callback_base::finish();

		// Fix up

		if (!found_after_end)
		{
			end_pos=i;
		}
	}

};
#if 0
{
#endif
}

std::tuple<size_t, size_t>
richtext_range::select_word(const richtextfragmentObj *f, size_t pos)
{
	auto &meta=f->string.get_meta();

	auto beg_iter=richtextstring::meta_upper_bound_by_pos(meta, pos);

	if (beg_iter == meta.begin())
		throw EXCEPTION("Internal error: iterator not found in"
				" select_word()");

	--beg_iter;

	// If pos is in the left-to-right text find where it begins, ditto
	// for right-to-left.

	auto rl=beg_iter->second.rl;

	while (beg_iter != meta.begin())
	{
		--beg_iter;
		if (beg_iter->second.rl != rl)
		{
			++beg_iter;
			break;
		}
	}

	auto end_iter=beg_iter;

	// Ditto: where it ends

	while (++end_iter != meta.end())
	{
		if (end_iter->second.rl != rl)
			break;
	}

	// beg_iter and end_iter now define the sequence of characters in the
	// same rendering direction that we will run through the unicode
	// word breaking algorithm.

	assert_or_throw(beg_iter->first < f->string.size() &&
			beg_iter->first <= pos &&
			(end_iter == meta.end() ||
			 end_iter->first < f->string.size()),
			"internal error: inconsistent meta in select_word()");

	size_t starting_index=beg_iter->first;

	auto &string=f->string.get_string();

	auto b=string.begin()+beg_iter->first;
	auto e=end_iter == meta.end() ? string.end()
		: string.begin()+end_iter->first;

	size_t nchars=e-b;

	find_word search{
		rl
		// Below, we'll be scanning from the right
		//
		// So, if we're scanning characters 4-6, and pos is 6, from
		// find_word's perspective we are looking for position 0.

		? starting_index+nchars-1-pos
		: pos-starting_index};
	if (rl)
	{
		while (b != e)
			search << *--e;
	}
	else
	{
		while (b != e)
			search << *b++;
	}

	search.finish();

	// Now we must now translate the relative offsets in find_word to
	// the real ones.

	size_t begin_pos;
	size_t end_pos;

	if (rl)
	{
		// Right-to-left text:
		//
		// starting_index+nchars-1 is the index of the first character
		// that was laundered through the word-breaking algorithm,
		// so character #n would be
		//
		// starting_index+nchars-1-begin_pos
		//
		// That's going to really be the LAST character, so ending_pos
		// is that plus + 1:

		end_pos=starting_index+nchars-search.begin_pos;

		//
		// And one past the end would be
		//
		// starting_index+chars-1-end_pos
		//
		// So the starting index is that plus 1

		begin_pos=starting_index+nchars-search.end_pos;
	}
	else
	{
		// This one's easy, just add the starting_index

		begin_pos=search.begin_pos + starting_index;
		end_pos=search.end_pos + starting_index;
	}


	// Ok, did we find a word?
	if (end_pos > begin_pos)
	{
		// Make end_pos point to the last character in the word.

		// Then, adjust for the fragment's text direction.
		--end_pos;

		if (f->embedding_level() == UNICODE_BIDI_LR)
		{
			if (end_pos+1 < string.size())
				// Should be the case
				++end_pos;
		}
		else
		{
			std::swap(begin_pos, end_pos);

			if (end_pos > 0) // Should be the case
				--end_pos;
		}
	}

	return {begin_pos, end_pos};
}

std::tuple<richtextiterator, richtextiterator>
richtext_range::replace_hotspot_iterators(ONLY IN_THREAD,
					  const richtext &me,
					  const ref<richtext_implObj> &impl,
					  const text_hotspot &hotspot)
{
	// Find the hotspot's first and last fragment

	auto iter=impl->hotspot_collection.find(hotspot);

	if (iter == impl->hotspot_collection.end())
		throw EXCEPTION("Internal error: existing hotspot not found.");

	auto &[begin_fragment, end_fragment]=iter->second;

	// In the first and the last fragment find where
	// the hotspot begin or ends.

	auto begin_hotspot_iter=
		begin_fragment->hotspot_collection.find(hotspot);

	if (begin_hotspot_iter ==
	    begin_fragment->hotspot_collection.end())
		throw EXCEPTION("Internal error: hotspot start not found.");

	auto end_hotspot_iter=
		end_fragment->hotspot_collection.find(hotspot);

	if (end_hotspot_iter ==
	    end_fragment->hotspot_collection.end())
		throw EXCEPTION("Internal error: hotspot end not found.");

	// Determine, in the first and last fragment,
	// the position where the hotspot starts and ends,
	// based on the corresponding fragment's embedding_level().

	auto &[begin_start, begin_end]=begin_hotspot_iter->second;
	auto &[end_start, end_end]=end_hotspot_iter->second;

	auto loc1=richtextcursorlocation::create();
	auto loc2=richtextcursorlocation::create();

	if (begin_fragment == end_fragment)
	{
		// begin and end should be the same.
		//
		// There should be at least two hotspot_markers

		if (begin_end < begin_start || begin_end - begin_start < 2)
			throw EXCEPTION("Internal error: hotspot not marked");

		// Create the iterators pointing to the beginning and the
		// ending marker.
		std::tuple iterators{
			richtextiterator::create(me, loc1,
						 &*begin_fragment,
						 begin_start,
						 new_location::lr),
			richtextiterator::create(me, loc2,
						 &*begin_fragment,
						 begin_end-1,
						 new_location::lr),
		};

		// If this is going to be left-to-right text, the ending
		// richtextiterator already points one-past-the-last character
		// of the hotspot, so we need to just advance the beginning
		// richtextiterator to the first character in the hotspot.
		//
		// For right-to-left text it's the mirror opposite.
		if (begin_fragment->embedding_level() == UNICODE_BIDI_LR)
		{
			std::get<0>(iterators)->right(IN_THREAD);
		}
		else
		{
			std::get<1>(iterators)->left(IN_THREAD);
		}

		return iterators;
	}

	// Hotspot begins and ends on different lines.
	auto top_embedding_level=begin_fragment->embedding_level();
	auto bottom_embedding_level=end_fragment->embedding_level();

	auto top_iter=
		richtextiterator::create(me, loc1, &*begin_fragment,
					 top_embedding_level
					 == UNICODE_BIDI_LR
					 ? begin_start : begin_end-1,
					 new_location::lr);

	// Move the top iterator to the first character of the hotspot text,
	// according to the fragment's embedding level.
	if (top_embedding_level == UNICODE_BIDI_LR)
		top_iter->right(IN_THREAD);
	else
		top_iter->left(IN_THREAD);

	// The ending iterator:
	//
	// If the bottom fragment is left-to-right, position the
	// richtextiterator on the ending marker character. Otherwise the
	// first character in the hotspot range must be the ending marker,
	// and it will be one-past the last character in this right-to-left
	// fragment.
	auto bottom_iter=
		richtextiterator::create(me, loc2, &*end_fragment,
					 bottom_embedding_level
					 == UNICODE_BIDI_LR
					 ? end_end-1:end_start,
					 new_location::lr);

	return {top_iter, bottom_iter};
}

void richtext_range::rl_lines(const richtextfragmentObj *bottom,
			      const richtextfragmentObj *top,
			      const richtextfragmentObj *last_lr)
{
	if (paragraph_embedding_level == UNICODE_BIDI_LR)
	{
		assert_or_throw(!last_lr,
				"Internal error: unexpected lr text "
				"after rl text in lr paragraph");
		rl_lines_in_lr(bottom, top);
		return;
	}

	if (last_lr)
	{
		// If we skipped over some
		// left-to-right lines, emit
		// them.

		lr_lines_in_rl(bottom->next_fragment(),
			       last_lr);
	}

	while (1)
	{
		line(bottom);

		if (compare_fragments(bottom, top))
			break;
		bottom=bottom->prev_fragment();
	}
}


void richtext_range::rl_lines_in_lr(const richtextfragmentObj *bottom,
				    const richtextfragmentObj *top)
{
	while(1)
	{
		assert_or_throw(bottom != 0,
				"Internal error: null rl_line");
		line(bottom);

		if (compare_fragments(bottom, top))
			break;
		bottom=bottom->prev_fragment();
	}
}

void richtext_range::lr_lines(const richtextfragmentObj *first_rl,
			      const richtextfragmentObj *top,
			      const richtextfragmentObj *bottom)
{
	if (paragraph_embedding_level != UNICODE_BIDI_LR)
	{
		assert_or_throw(!first_rl,
				"Internal error: unexpected rl text "
				"before lr text in rl paragraph");

		lr_lines_in_rl(top, bottom);
		return;
	}

	// If the first line contains the starting range of extracted
	// text, call line() normally to truncate it.

	if (compare_fragments(top, location_a->my_fragment))
	{
		assert_or_throw(!first_rl,
				"Internal error: unexpected rl text "
				"before start of extracted text");

		line(top);
	}
	else
	{
		// If we just seen at least one right to
		// left line, process them.
		//
		// Before processing all the pure
		// right-to-left lines, if the
		// first left-to-right line has some
		// leading right-to-left text, it's
		// part of the right-to-left sequence
		// and since right-to-left text is
		// processed from bottom up we just
		// handle it ourselves, here.
		//
		// But first, also check if the entire
		// get() range also ends on this
		// fragment.
		size_t cutoff=top->string.size();

		if (compare_fragments
		    (top, location_b->my_fragment))
		{
			cutoff=location_b->get_offset();
		}

		// If the line starts with some
		// right to left text, find where
		// left to right text starts.

		auto m=top->string.get_meta();
		auto b=m.begin(),
			e=m.end();

		while (b != e && b->second.rl)
			++b;

		// Ok, so if right-to-left text
		// ends before the cutoff, we emit
		// it in its entirety here, before
		// emitting the rest of rl_lines.
		// Otherwise we emit only up to the
		// cutoff.
		add(top->string, 0,
		    b->first < cutoff ?
		    b->first : cutoff);

		if (first_rl)
		{

			// Now, emit all right-to-left lines.
			rl_lines(top->prev_fragment(),
				 first_rl, nullptr);
		}
		// And then emit everything on this
		// line after the end of the right
		// to left text, and before the cutoff
		// (the end of the line, or the end
		// of the get() range).
		if (cutoff > b->first)
			add(top->string, b->first,
			    cutoff-b->first);
	}

	// Process any remaining left-to-right lines normally.
	while (!compare_fragments(top, bottom))
	{
		top=top->next_fragment();

		// Ordinary left to right line. Boring.
		line(top);
	}
}

void richtext_range::lr_lines_in_rl(const richtextfragmentObj *top,
				    const richtextfragmentObj *bottom)
{
	// The bottom line may not be entirely left-to-right. The
	// left to right portion would be at the end of the line.
	//
	// Start from the end of the line, and find where the left
	// to right text begins.

	auto &m=bottom->string.get_meta();
	auto b=m.begin(), e=m.end();

	while (b != e && !e[-1].second.rl)
		--e;

	// Right to left text begins to the right of "rl_begin".
	size_t rl_begin = e == m.end() ? bottom->string.size():e->first;

	// If bottom happens to be the end location of the get()
	// range, note the offset in the bottom fragment.
	//
	// Also, make a copy of location_b AND RESTORE IT before
	// we return, the logic below may modify it temporarily.

	const size_t orig_location_b_offset=location_b_offset;

	// If "bottom" is the fragment with the end of the get()
	// range, and the end of the get() range is to the left
	// of rl_begin, it's grabbing some right-to-left text, and
	// since we are extracting the right-to-left text from the
	// bottom and going our way up, we need to grab the
	// portion between location_b and rl_begin, as the first
	// order of business.
	//
	// We'll then temporary move location_b, the ending get()
	// location, to rl_begin, so that the existing logic
	// below, when it eventually gets called for the bottom
	// line, ends up extracting just the left-to-right text.

	if (compare_fragments(bottom, location_b->my_fragment) &&
	    orig_location_b_offset < rl_begin)
	{
		add(bottom->string, orig_location_b_offset+1,
		    rl_begin-(orig_location_b_offset+1));

		// We need to have the temporary location_b_offset begin
		// just to the left of rl_begin, since the logic
		// in line() trims off everything from the beginnin
		// of the line up to and including the ending
		// position.

		location_b_offset=rl_begin-1;
	}

	// With that out of the way, extract the lines of left-to-right
	// text, from top to bottom.

	while(1)
	{
		assert_or_throw(top != 0,
				"Internal error: null rl_line");

		// If this line is the bottom line of the get() range
		// we only need to extract everything on or after
		// the ending position.

		if (compare_fragments(top, location_b->my_fragment))
		{
			size_t end=location_b_offset+1;
			add(top->string,
			    end,
			    top->string.size()-end);
		}
		else if (compare_fragments(top,
					   location_a->my_fragment))
		{
			line(top);
		}
		else
		{
			line(top);
		}
		if (compare_fragments(bottom, top))
			break;
		top=top->next_fragment();
	}

	location_b_offset=orig_location_b_offset;
}

void richtext_range::line(const richtextfragmentObj *f)
{
	if (compare_fragments(f, location_a->my_fragment))
	{
		// Starting location.

		auto o=location_a_offset;

		if (f->embedding_level() == UNICODE_BIDI_LR)
		{
			// For left-to right text we add() only from the
			// starting location to the end of the line.
			add(f->string, o, f->string.size()-o);
		}
		else
		{
			// For right to left text we add from the
			// beginning of the line up to *and including*
			// the starting location. So we add +1 to o.
			add(f->string, 0, o+1);
		}
		return;
	}

	if (compare_fragments(f, location_b->my_fragment))
	{
		// Ending location
		auto o=location_b_offset;

		if (paragraph_embedding_level == UNICODE_BIDI_LR)
		{
			// Left to right text, add() only from the
			// beginning of the line to the ending location.
			add(f->string, 0, o);
		}
		else
		{
			// Right to left text, so to the right of 'o'
			// is what we get(), starting at o+1.
			add(f->string, o+1, f->string.size()-(o+1));
		}
		return;
	}
	add(f->string, 0, f->string.size());
}

void richtext_range::add(const richtextstring &other,
			 size_t start,
			 size_t n) const
{
	size_t s=other.size();

	assert_or_throw(start <= s &&
			(s-start) >= n,
			"Internal error: invalid get string offset");

	range(other, start, n);
}

LIBCXXW_NAMESPACE_END
