/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextparagraph.H"
#include "richtext/fragment_list.H"
#include "x/w/impl/richtext/richtext.H"
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

bool richtextparagraphObj::rewrap(paragraph_list &my_paragraphs,
				  dim_t width)
{
	fragment_list my_fragments(my_paragraphs, *this);

	size_t my_fragment_n=0;
	bool changed=false;

	while (my_fragment_n < my_fragments.size())
	{
		bool toosmall, toobig;

		rewrap_fragment(my_fragments,
				width, my_fragment_n, toosmall, toobig);
		++my_fragment_n;

		if (toosmall || toobig)
			changed=true;
	}

	if (changed)
		my_fragments.fragments_were_rewrapped();

	return changed;
}

bool richtextparagraphObj::unwrap(paragraph_list &my_paragraphs)
{
	fragment_list my_fragments(my_paragraphs, *this);

	if (my_fragments.size() == 0)
		return false;

	bool flag=false;

	// When the paragraph embedding level is left-to-right, we need to
	// merge all right-to-left fragments first.
	//
	// When the paragraph embedding level is right-to-left, we need to
	// merge all left-to-right fragments first.

	auto merge_first=my_richtext->paragraph_embedding_level ==
		UNICODE_BIDI_LR ? richtext_dir::rl : richtext_dir::lr;

	size_t fs=my_fragments.size();

	for (size_t n=0; n+1<fs; )
	{
		auto f=get_fragment(n);

		if (f->string.get_dir() != merge_first)
		{
			++n;
			continue;
		}

		f->merge(my_fragments);
		flag=true;
		fs=my_fragments.size();
	}

	// And then, merge the rest.
	while (my_fragments.size() > 1)
	{
		get_fragment(0)->merge(my_fragments);
		flag=true;
	}

	if (flag)
		my_fragments.fragments_were_rewrapped();
	return flag;
}

void richtextparagraphObj::rewrap_fragment(fragment_list &my_fragments,
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

	while (fragment_n+1 < my_fragments.size())
	{
		// Is the next fragment starts with something small enough
		// to get merged into here?

		auto next_fragment=*get_fragment_iter(fragment_n+1);
		auto initial_width=next_fragment->initial_width;

		// If this fragment is rl, the next fragment's initial_width
		// will work for us, either way. If the next fragment starts
		// with rl text, this is going to be the last rl text segment.
		//
		// If this fragment is not rl, and the next fragment begins
		// with rl text, it must fit.

		if ((*iter)->string.get_dir() != richtext_dir::rl)
		{
			size_t p=next_fragment->string.left_to_right_start();

			if (p > 0)
			{
				initial_width=p >= next_fragment->string.size()
					? next_fragment->horiz_info.width()
					: next_fragment->horiz_info.x_pos(p);
			}
		}
		if ((*iter)->width + initial_width > wwidth)
			break; // It would be too big.

		// We'll merge the whole thing, and sort things out later.
		(*iter)->merge(my_fragments);
		iter=get_fragment_iter(fragment_n);
		toosmall=true;

		// Need to compute updated fragment sizes in order to
		// continue.
		my_fragments.recalculate_needed_fragment_sizes();
	}

	if ( (*iter)->width <= dim_t::value_type(width))
		return; // We're fine.

	assert_or_throw(! (*iter)->horiz_info.empty(),
			"How can we be so big, when there are no widths?");

	size_t n=(*iter)->horiz_info.size();

	assert_or_throw(n == (*iter)->breaks.size(),
			"Mismatch between string size and horizontal metrics");

	auto split_type=richtextfragmentObj::split_lr;

	if ((*iter)->string.get_dir() == richtext_dir::rl)
	{
		// Because we're not fine, see above, this subtraction is valid.
		wwidth=dim_t::value_type((*iter)->width) -
			dim_t::value_type(width);
		split_type=richtextfragmentObj::split_rl;
	}

	std::optional<size_t> last_break_pos;

	dim_squared_t accumulated_width=0;

	auto &meta=(*iter)->string.get_meta();

	auto metap=meta.begin();
	auto metae=meta.end();
	auto next_metap=metap;

	if (next_metap != metae)
		++next_metap;

	for (size_t i=0; i<n; ++i)
	{
		auto cur_break=(*iter)->breaks[i];

		// If we're NOT splitting an entire fragment of rl text, we
		// will NOT consider a break point in rl text.
		//
		// We make this check BEFORE we check if this index starts
		// a new meta. There should be a break when rl text starts,
		// so we should break there.

		if (split_type == richtextfragmentObj::split_lr &&
		    metap != metae && metap->second.rl)
			cur_break=unicode_lb::none;

		// Keep track of the current metadata, as we go along.

		if (next_metap != metae && next_metap->first == i)
		{
			metap=next_metap;
			++next_metap;

			// And when rl text ends, there should be a break there
			// too.
			if (!metap->second.rl)
				cur_break=(*iter)->breaks[i];
		}

		// Checkpoint each breakable character.

		if (i && cur_break != unicode_lb::none)
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

		accumulated_width +=
			(*iter)->horiz_info.width(i)+
			(*iter)->horiz_info.kerning(i);
	}

	// At this point, break at the last position that fell below the
	// requested width. If there were no break positions, we're boned.

	if (last_break_pos)
	{
		(*iter)->split(my_fragments, *last_break_pos, split_type);

		iter=get_fragment_iter(fragment_n);

		if (!toosmall) // Don't complain twice.
			toobig=true;

		++iter;

		// Need to compute updated fragment sizes in order to
		// continue.
		my_fragments.recalculate_needed_fragment_sizes();
	}
}

richtextfragment richtextparagraphObj::find_fragment_for_pos(size_t &pos) const
{
	return fragments.find_fragment_for_pos(pos);
}

LIBCXXW_NAMESPACE_END
