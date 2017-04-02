/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextparagraph.H"
#include "richtext/fragment_list.H"
#include "richtext/richtext.H"
#include "richtext/paragraph_list.H"
#include "assert_or_throw.H"

LIBCXXW_NAMESPACE_START

richtextparagraphObj::richtextparagraphObj()
{
}

richtextparagraphObj::~richtextparagraphObj()
{
	fragments.paragraph_destroyed();
}

bool richtextparagraphObj::rewrap(IN_THREAD_ONLY,
				  paragraph_list &my_paragraphs,
				  dim_t width)
{
	fragment_list my_fragments(my_paragraphs, *this);

	size_t my_fragment_n=0;
	bool changed=false;

	while (my_fragment_n < my_fragments.size())
	{
		bool toosmall, toobig;

		rewrap_fragment(IN_THREAD, my_fragments,
				width, my_fragment_n, toosmall, toobig);
		++my_fragment_n;

		if (toosmall || toobig)
			changed=true;
	}

	if (changed)
		my_fragments.fragments_were_rewrapped();

	return changed;
}

bool richtextparagraphObj::unwrap(IN_THREAD_ONLY,
				  paragraph_list &my_paragraphs)
{
	fragment_list my_fragments(my_paragraphs, *this);

	if (my_fragments.size() == 0)
		return false;

	bool flag=false;

	while (my_fragments.size() > 1)
	{
		get_fragment(0)->merge(IN_THREAD, my_fragments);
		flag=true;
	}

	if (flag)
		my_fragments.fragments_were_rewrapped();
	return flag;
}

void richtextparagraphObj::rewrap_fragment(IN_THREAD_ONLY,
					   fragment_list &my_fragments,
					   dim_t width,
					   size_t fragment_n,
					   bool &toosmall,
					   bool &toobig)
{
	toosmall=false;
	toobig=false;

	// See if this fragment is small enough that at least the initial
	// part of the next fragment can be attached to this one.

	auto iter=get_fragment_iter(fragment_n);

	dim_squared_t wwidth=dim_t::value_type(width);

	while (fragment_n+1 < my_fragments.size() &&
	       (*iter)->width
	       + (*get_fragment_iter(fragment_n+1))->minimum_width <= wwidth)
	{
		// We'll merge the whole thing, and sort things out later.
		(*iter)->merge(IN_THREAD, my_fragments);
		iter=get_fragment_iter(fragment_n);
		toosmall=true;
	}

	if ( (*iter)->width <= dim_t::value_type(width))
		return; // We're fine.

	assert_or_throw(! (*iter)->horiz_info.empty(),
			"How can we be so big, when there are no widths?");

	size_t last_break_pos=0;

	dim_squared_t accumulated_width=0;

	size_t n=(*iter)->horiz_info.size();
	for (size_t i=0; ; ++i)
	{
		// Checkpoint at each breakable character, and at the end of the
		// fragment.

		if (i && (i == n || (*iter)->breaks[i] != UNICODE_LB_NONE))
		{
			if (accumulated_width-
			    (*iter)->horiz_info.kerning(0) // Doesn't count
			    > wwidth

			    // If the initial fragment is too big for the
			    // width, just break at it, as a last resort
			    && last_break_pos)
				break;
			last_break_pos=i;
		}

		if (i == n)
			break;

		accumulated_width += (*iter)->horiz_info.width(i)+(*iter)->horiz_info.kerning(i);
	}

	// At this point, break at the last position that fell below the
	// requested width. If there were no break positions, we're boned.

	if (last_break_pos && last_break_pos < n)
	{
		(*iter)->split(IN_THREAD, my_fragments, last_break_pos);

		iter=get_fragment_iter(fragment_n);

		if (!toosmall) // Don't complain twice.
			toobig=true;

		++iter;
	}
}

void richtextparagraphObj::insert(IN_THREAD_ONLY,
				  paragraph_list &my_paragraphs,
				  size_t pos,
				  const richtextstring &string)
{
	auto fragment=find_fragment_for_pos(pos);

	fragment->insert(IN_THREAD, my_paragraphs, pos, string);
}

richtextfragment richtextparagraphObj::find_fragment_for_pos(size_t &pos) const
{
	return fragments.find_fragment_for_pos(pos);
}

LIBCXXW_NAMESPACE_END
