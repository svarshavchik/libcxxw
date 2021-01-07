/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_range.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtextfragment.H"
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

		if (paragraph_embedding_level == UNICODE_BIDI_LR)
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
