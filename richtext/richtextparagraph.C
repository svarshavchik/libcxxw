/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextparagraph.H"
#include "richtext/fragment_list.H"
#include "richtext/richtext_insert.H"
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
				  dim_t width,
				  richtext_insert_results &results)
{
	fragment_list my_fragments(my_paragraphs, *this);

	size_t my_fragment_n=0;
	bool changed=false;

	while (my_fragment_n < my_fragments.size())
	{
		bool toosmall, toobig;

		rewrap_fragment(my_fragments,
				width, my_fragment_n, toosmall, toobig,
				results);
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

	richtext_insert_results ignored;

	for (size_t n=0; n+1<fs; )
	{
		auto f=get_fragment(n);

		if (f->string.get_dir() != merge_first)
		{
			++n;
			continue;
		}

		f->merge(my_fragments, f->merge_bidi, ignored);
		flag=true;
		fs=my_fragments.size();
	}

	// And then, merge the rest.
	while (my_fragments.size() > 1)
	{
		auto f=get_fragment(0);

		f->merge(my_fragments, f->merge_bidi, ignored);
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
					   bool &toobig,
					   richtext_insert_results
					   &insert_results)
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
		(*iter)->merge(my_fragments, (*iter)->merge_bidi,
			       insert_results);
		iter=get_fragment_iter(fragment_n);
		toosmall=true;

		// Need to compute updated fragment sizes in order to
		// continue.
		my_fragments.recalculate_needed_fragment_sizes();
	}

	// We may get several cracks at it. The fragment may have both
	// left to right and right to left text. We don't split fragments
	// whose rendering order is opposite the paragraph rendering order,
	// unless the entire line is opposite rendering order.
	//
	// So the initial split of a fragment thta contains both rendering order
	// may end up with a fragment solely in the opposite rendering order,
	// but then it can be split again.

	while ( (*iter)->width > dim_t::value_type(width))
	{
		size_t n=(*iter)->string.size();

		if (n <= 1)
			break;

		richtextfragmentObj *split_nl=nullptr;
		bool split_nl_rl=false;
		dim_t newline_width=0;

		auto &s=(*iter)->string.get_string();

		// If the line fragment ends or begins with a newline this
		// is the last fragment in the paragraph (left to right or
		// right to left embedding level).
		//
		// We temporarily split off the newline into the following
		// fragment, then whatever happens will move it back.
		//
		// split_nl is the split fragment containing the newline.
		//
		// We also make note of the width of the newline itself.

		if (s[n-1] == '\n')
		{
			// left to right paragraph end sin a newline.

			newline_width=dim_t::truncate
				( (*iter)->horiz_info.width(n-1) +
				  (*iter)->horiz_info.kerning(n-1));

			(*iter)->split(my_fragments, n-1,
				       richtextfragmentObj::split_lr, true,
				       insert_results);
			iter=get_fragment_iter(fragment_n);

			split_nl=(*iter)->next_fragment();
		}
		else if (s[0] == '\n')
		{
			newline_width=(*iter)->horiz_info.width(0);

			(*iter)->split(my_fragments, 1,
				       richtextfragmentObj::split_rl, true,
				       insert_results);
			split_nl_rl=true;
			iter=get_fragment_iter(fragment_n);
			split_nl=(*iter)->next_fragment();
		}

		if (split_nl)
			my_fragments.recalculate_needed_fragment_sizes();

		auto res=rewrap_fragment_toobig(my_fragments,
						*iter, width,
						toosmall, toobig,
						insert_results);

		// If we failed, it's possible that after we restore the \n
		// we'll be too big. So try again to get below
		// width-newline_width. Maybe we were too big because of the
		// newline
		if (res == toobig_results::failed && split_nl)
			res=rewrap_fragment_toobig(my_fragments,
						   *iter, width-newline_width,
						   toosmall, toobig,
						   insert_results);

		// Now, no matter what, put the newline back where it came
		// from.
		if (split_nl)
		{
			auto prev=split_nl->prev_fragment();

			assert_or_throw(prev,
					"Internal error: cannot find prev "
					"fragment to a newline");

			if (split_nl_rl)
			{
				// Newline at the beginning of the line,
				// right to left paragraph embedding level.
				split_nl->merge_lr_lr(my_fragments,
						      ref{prev},
						      insert_results);
			}
			else
			{
				// Newline at the end of the line,
				// left to right paragraph embedding level.
				prev->merge_lr_lr(my_fragments,
						  ref{split_nl},
						  insert_results);
			}
			// Need to compute updated fragment sizes in order to
			// continue.

			my_fragments.recalculate_needed_fragment_sizes();
		}
		if (res != toobig_results::tryagain)
			break;
		iter=get_fragment_iter(fragment_n);
	}
}

richtextparagraphObj::toobig_results
richtextparagraphObj::rewrap_fragment_toobig(fragment_list &my_fragments,
					     const richtextfragment &f,
					     dim_t width,
					     bool &toosmall,
					     bool &toobig,
					     richtext_insert_results
					     &insert_results)
{
	assert_or_throw(! f->horiz_info.empty(),
			"How can we be so big, when there are no widths?");

	size_t n=f->horiz_info.size();

	assert_or_throw(n == f->breaks.size(),
			"Mismatch between string size and horizontal metrics");

	size_t pos;

	// Figure out the split type.

	auto split_type=richtextfragmentObj::split_lr;

	auto orig_dir=f->string.get_dir();

	switch (orig_dir) {
	case richtext_dir::lr:
		break;
	case richtext_dir::rl:
		split_type=richtextfragmentObj::split_rl;
		break;
	case richtext_dir::both:
		if (my_richtext->paragraph_embedding_level != UNICODE_BIDI_LR)
		{
			split_type=richtextfragmentObj::split_rl;
		}
		break;
	default:
		assert_or_throw(false,
				"Unknown bidirectional fragment type.");
		return toobig_results::failed;
	}

	if (split_type == richtextfragmentObj::split_rl)
	{
		pos=f->compute_split_pos_rl(width);
	}
	else
	{
		pos=f->compute_split_pos_lr(width);
	}

	if (pos == 0 || pos >= f->string.size())
		return toobig_results::failed;

	f->split(my_fragments, pos, split_type, false, insert_results);

	if (!toosmall) // Don't complain twice.
		toobig=true;

	// Need to compute updated fragment sizes in order to
	// continue.

	my_fragments.recalculate_needed_fragment_sizes();

	return orig_dir == richtext_dir::both
		? toobig_results::tryagain : toobig_results::success;
}

richtextfragment richtextparagraphObj::find_fragment_for_pos(size_t &pos) const
{
	return fragments.find_fragment_for_pos(pos);
}

LIBCXXW_NAMESPACE_END
