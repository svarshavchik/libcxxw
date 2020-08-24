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
		if ((*iter)->width + (*iter)->compute_initial_width_for_bidi()
		    > wwidth)
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

	size_t pos;

	auto split_type=richtextfragmentObj::split_lr;

	switch ((*iter)->string.get_dir()) {
	case richtext_dir::lr:
		pos=(*iter)->compute_split_pos_lr(width);
		break;
	case richtext_dir::rl:
		pos=(*iter)->compute_split_pos_rl(width);

		split_type=richtextfragmentObj::split_rl;
		break;
	case richtext_dir::both:
		if (my_richtext->paragraph_embedding_level == UNICODE_BIDI_LR)
		{
			pos=(*iter)->compute_split_pos_lr(width);
		}
		else
		{
			pos=(*iter)->compute_split_pos_rl(width);
			split_type=richtextfragmentObj::split_rl;
		}
		break;
	default:
		assert_or_throw(false,
				"Unknown bidirctional fragment type.");
		return;
	}

	if (pos == 0 || pos >= (*iter)->string.size())
		return;

	(*iter)->split(my_fragments, pos, split_type);

	if (!toosmall) // Don't complain twice.
		toobig=true;

	// Need to compute updated fragment sizes in order to
	// continue.

	my_fragments.recalculate_needed_fragment_sizes();
}

richtextfragment richtextparagraphObj::find_fragment_for_pos(size_t &pos) const
{
	return fragments.find_fragment_for_pos(pos);
}

LIBCXXW_NAMESPACE_END
