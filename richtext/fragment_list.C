/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtext_impl.H"
#include "x/w/defaultthemefwd.H"
#include <algorithm>

LIBCXXW_NAMESPACE_START

const_fragment_list::const_fragment_list(paragraph_list &my_paragraphsArg,
					 richtextparagraphObj &paragraphArg)
	: my_paragraphs(my_paragraphsArg), paragraph(paragraphArg)
{
}

const_fragment_list::~const_fragment_list()
{
	// Make sure this paragraph is frobbed correctly, then
	// keep frobbing the remaining paragraphs.

	paragraph.n_fragments_in_paragraph=
		paragraph.fragments.size();

	size_t first_fragment_n=paragraph.first_fragment_n +
		paragraph.n_fragments_in_paragraph;

	size_t first_fragment_y_position=paragraph.next_paragraph_y_position();

	// Examine the following paragraphs. If their vertical position has
	// changed, update them.

	paragraph.my_richtext->paragraphs.for_paragraphs
		(paragraph.my_paragraph_number+1,
		 [&, this]
		 (const richtextparagraph &b)
		 {
			 auto &p=*b;

			 size_t n_fragments_in_paragraph=p.fragments.size();

			 if (first_fragment_n == p.first_fragment_n &&
			     n_fragments_in_paragraph ==
			     p.n_fragments_in_paragraph &&
			     first_fragment_y_position ==
			     p.first_fragment_y_position)
				 // No need to go any further, assuming frobbing
				 // is being diligently followed.
				 return false;

			 p.first_fragment_y_position=first_fragment_y_position;
			 p.first_fragment_n=first_fragment_n;
			 p.n_fragments_in_paragraph=n_fragments_in_paragraph;
			 first_fragment_n += n_fragments_in_paragraph;
			 first_fragment_y_position=
				 p.next_paragraph_y_position();
			 my_paragraphs.size_changed=true;
			 return true;
		 });

	if (paragraph.fragments.empty()) // remove()d everything
	{
		paragraph.my_richtext=nullptr;
		my_paragraphs.erase(paragraph.my_paragraph_number);
	}
}


void const_fragment_list
::insert_no_recalculate(fragments_t::iterator at,
			const richtextfragment &fragment)
	const
{
	auto iter=paragraph.fragments.insert(at, fragment);

	fragment->my_fragment_number=iter-paragraph.fragments.begin();

	auto e=paragraph.fragments.end();

	while (++iter != e)
		++(*iter)->my_fragment_number;

	fragment->my_paragraph=&paragraph;
}

// This is used in set(). The fragment doesn't have any text yet, it must
// be linked into the paragraph, before it is loaded.

void const_fragment_list
::append_no_recalculate(const richtextfragment &new_fragment)
	const
{
	insert_no_recalculate(paragraph.fragments.end(), new_fragment);
	paragraph.adjust_char_count(new_fragment->string.size());
}


fragment_list::fragment_list(paragraph_list &my_paragraphsArg,
			     richtextparagraphObj &paragraphArg)
	: const_fragment_list{my_paragraphsArg, paragraphArg}
{
}

fragment_list::~fragment_list()
{
	if (paragraph.my_richtext == nullptr) return;

	bool changed=false;

	if (size_changed)
		recalculate_needed_fragment_sizes();
	if (size_changed && recalculate_size())
		changed=true;

	if (changed)
		my_paragraphs.size_changed=true;
}

void fragment_list::theme_was_updated(ONLY IN_THREAD,
				      const const_defaulttheme &new_theme)
{
	paragraph.fragments.for_fragments
		([&]
		 (const richtextfragment &f)
		 {
			 f->theme_updated_called_by_fragment_list
				 (IN_THREAD, new_theme);
		 });

	recalculate_size();
}

void fragment_list::recalculate_needed_fragment_sizes()
{
	paragraph.fragments.for_fragments
		([&, this]
		 (const richtextfragment &f)
		 {
			 if (!f->recalculate_size_needed)
				 return;

			 f->recalculate_size_needed=false;
			 f->recalculate_size_called_by_fragment_list();
		 });
}

void fragment_list::insert_no_change_in_char_count(fragments_t::iterator at,
						   const richtextfragment
						   &fragment)
{
	insert_no_recalculate(at, fragment);
	recalculate_fragment_size(fragment->my_fragment_number);
	size_changed=true;
}

void fragment_list::erase(fragments_t::iterator at)
{
	for (auto p=at, e=paragraph.fragments.end(); ++p != e; )
		--(*p)->my_fragment_number;

	(*at)->my_paragraph=nullptr;
	paragraph.fragments.erase(at);
	size_changed=true;
}

void fragment_list::append(const richtextfragment &new_fragment)
{
	append_no_recalculate(new_fragment);
	recalculate_fragment_size(new_fragment->my_fragment_number);
	size_changed=true;
}

size_t fragment_list::size() const
{
	return paragraph.fragments.size();
}

void fragment_list::split_from(const richtextfragment &new_fragment,
			       richtextfragmentObj *split_after)
{
	// First order of business is to add the new fragment. This
	// adjust_char_count()s, this paragraph, on account of this fragment.

	append(new_fragment);

	// We added size() here, so subtract the same from over there.
	split_after->my_paragraph->adjust_char_count(-new_fragment->string
						     .size());

	// Make a copy of the fragments that are going to get moved.
	auto &old_fragments=split_after->my_paragraph->fragments;

	auto p=old_fragments.begin()+split_after->my_fragment_number;

	++p;

	auto e=old_fragments.end();

	std::vector<richtextfragment> copy(p, e);

	// The first order of business is to remove them from their current
	// paragraph, and adjust the current paragraph's character count.
	for (const auto &f:copy)
	{
		split_after->my_paragraph->adjust_char_count(-f->string.size());
		f->my_paragraph=nullptr;
	}

	old_fragments.erase(p, e);

	// We can now append them to us, the new paragraph, and adjust the
	// character count.
	for (const auto &f:copy)
		append_no_recalculate(f);
	size_changed=true;
}

size_t fragment_list::remove(size_t first_fragment,
			     size_t n_fragments,
			     richtextfragmentObj *locations_to)
{
	assert_or_throw(first_fragment < size(),
			"Invalid first fragment index passed to remove()");

	size_t todo=size() - first_fragment;

	if (todo > n_fragments)
		todo=n_fragments;

	auto b=paragraph.fragments.begin()+first_fragment;
	auto e=b+todo;

	// First get any location markers out of that fragment.

	for (auto p=b; p != e; ++p)
		(*p)->move_locations_to_another_fragment(locations_to);

	// Make a copy of the removed fragments, and remove them from the
	// paragraph.
	std::vector<richtextfragment> removed_fragments(b, e);
	paragraph.fragments.erase(b, e);

	for (const auto &p:removed_fragments)
	{
		// Accounting

		size_t nchars=p->string.size();

		paragraph.adjust_char_count(-nchars);

		p->my_paragraph=nullptr;
	}

	// More accounting

	b=paragraph.fragments.begin()+first_fragment;
	e=paragraph.fragments.end();

	for (auto p=b; p != e; ++p)
		(*p)->my_fragment_number -= todo;

	recalculate_size();

	return todo;
}

// Merge the next paragraph into this one.

void fragment_list::join_next()
{
	auto pr=paragraph.my_richtext->paragraphs
		.get_paragraph(paragraph.my_paragraph_number+1);

	assert_or_throw(!(*pr)->fragments.empty(),
			"Internal error: empty paragraph in join_next()");

	// Make sure an unlikely exception won't screw things up.
	auto next_paragraph=*pr;

	my_paragraphs.erase(paragraph.my_paragraph_number+1);
	next_paragraph->adjust_char_count(-next_paragraph->num_chars);

	// Need to clear the paragraph's fragments. Paragraph's
	// destructor zaps its fragments' my_paragraph.

	std::vector<richtextfragment> copy=next_paragraph->fragments;
	next_paragraph->fragments.clear();

	for (const auto &f:copy)
		f->my_paragraph=nullptr;

	for (const auto &f:copy)
	{
		// This increments this paragraphs char_count, that's another
		// reason we have to explicitly substract them, beforehand.
		append_no_recalculate(f);
	}
	size_changed=true;
}

void fragment_list::fragment_text_changed(size_t fragment_number,
					  ssize_t text_count_changed)
{
	recalculate_fragment_size(fragment_number);
	size_changed=true;

	paragraph.adjust_char_count(text_count_changed);
}

void fragment_list::recalculate_fragment_size(size_t fragment_number)
{
	assert_or_throw(fragment_number < paragraph.fragments.size(),
			"Invalid fragment_number in recalculat_fragments()");

	paragraph.fragments.begin()[fragment_number]
		->recalculate_size_needed=true;
	size_changed=true;
}

bool fragment_list::recalculate_size()
{
	bool width_changed;
	bool height_changed;

	recalculate_size(width_changed, height_changed);

	return width_changed || height_changed;
}

void fragment_list::recalculate_size(bool &width_changed,
				     bool &height_changed)
{
	auto old_width=paragraph.width;
	auto old_height=paragraph.above_baseline+paragraph.below_baseline;

	paragraph.width=0;
	paragraph.maximum_width_if_one_line=0;
	paragraph.height_of_tallest_fragment=0;
	paragraph.minimum_width=0;
	paragraph.above_baseline=0;
	paragraph.below_baseline=0;

	size_t first_char_n=0;
	size_t fragment_y_pos=0;

	for (const auto &fragment:paragraph.fragments)
	{
		if (fragment->minimum_width > paragraph.minimum_width)
			paragraph.minimum_width=fragment->minimum_width;

		fragment->first_char_n=first_char_n;
		fragment->y_pos=fragment_y_pos;

		first_char_n += fragment->string.size();
		fragment_y_pos += dim_t::value_type(fragment->height());

		paragraph.maximum_width_if_one_line =
			dim_t::truncate(paragraph.maximum_width_if_one_line
					+ fragment->width);

		// Width does not include the first character's kerning.

		if (!fragment->horiz_info.empty())
			paragraph.maximum_width_if_one_line
				+= fragment->horiz_info.kerning(0);

		auto h=fragment->height();

		if (h > paragraph.height_of_tallest_fragment)
			paragraph.height_of_tallest_fragment=h;
	}

	auto b=paragraph.fragments.begin(), e=paragraph.fragments.end();

	if (b != e)
	{
		paragraph.width= (*--e)->width;
		paragraph.above_baseline=(*e)->above_baseline;
		paragraph.below_baseline=(*e)->below_baseline;

		while (b != e)
		{
			if (paragraph.width < (*b)->width)
				paragraph.width=(*b)->width;
			paragraph.above_baseline =
				dim_t::truncate(paragraph.above_baseline +
						(*b)->height());
			++b;
		}
	}

	width_changed=old_width != paragraph.width;
	height_changed=old_height !=
		paragraph.above_baseline+paragraph.below_baseline;
}

LIBCXXW_NAMESPACE_END
