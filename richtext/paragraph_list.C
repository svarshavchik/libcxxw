/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/paragraph_list.H"
#include "richtext/fragment_list.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtext_insert.H"
#include "x/w/impl/richtext/richtext.H"
#include "assert_or_throw.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

paragraph_list::paragraph_list(richtext_implObj &textArg) : text(textArg)
{
}

paragraph_list::~paragraph_list()
{
	if (size_changed)
		recalculate_size();
}

// This is used by set() to create a new paragraph

richtextparagraph paragraph_list::append_new_paragraph()
{
	auto new_paragraph=richtextparagraph::create();

	// Add paragraph
	text.paragraphs.push_back(new_paragraph);
	new_paragraph->my_richtext=&text;
	new_paragraph->my_paragraph_number=text.paragraphs.size()-1;

	if (new_paragraph->my_paragraph_number > 0)
	{
		// This calculates new paragraph's starting metrics, using
		// the code in fragment_list's destructor.

		const_fragment_list previous_paragraph_fragments
			(*this,
			 **text.paragraphs
			 .get_paragraph(new_paragraph->my_paragraph_number-1));

	}
	size_changed=true;
	return new_paragraph;
}

richtextparagraph paragraph_list::insert_new_paragraph(size_t insert_pos)
{
	assert_or_throw(insert_pos <= text.paragraphs.size(),
			"Internal error: invalid paragraph insert positoin");

	auto new_paragraph=richtextparagraph::create();

	text.paragraphs.insert(text.paragraphs.begin()+insert_pos,
			       new_paragraph);

	new_paragraph->my_richtext=&text;
	new_paragraph->my_paragraph_number=insert_pos;

	if (insert_pos > 0)
	{
		// Update cached data.

		auto &prev=text.paragraphs[insert_pos-1];

		new_paragraph->first_fragment_y_position=
			prev->next_paragraph_y_position();
		new_paragraph->first_fragment_n=
			prev->first_fragment_n +
			prev->n_fragments_in_paragraph;
	}

	text.paragraphs.for_paragraphs(insert_pos+1,
				       []
				       (const richtextparagraph &p)
				       {
					       ++p->my_paragraph_number;
					       return true;
				       });

	// We don't have to worry about the new paragraph's starting metrics.
	// There's a fragment_list lock on the previous paragraph's fragments,
	// and its destructor will take care of this.

	return new_paragraph;
}

void paragraph_list::erase(size_t paragraph_number)
{
	assert_or_throw(paragraph_number < text.paragraphs.size(),
			"Internal error: invalid paragraph number in erase()");

	text.paragraphs.erase(text.paragraphs.begin()+paragraph_number);

	text.paragraphs.for_paragraphs(paragraph_number,
				       [&]
				       (const richtextparagraph &p)
				       {
					       --p->my_paragraph_number;
					       return true;
				       });
}

void paragraph_list::recalculate_size()
{
	text.minimum_width=0;
	text.real_width=0;
	text.real_maximum_width=0;
	text.height_of_tallest_fragment=0;
	text.above_baseline=0;
	text.below_baseline=0;
	size_t first_char_n=0;

	for (const auto &paragraph:text.paragraphs)
	{
		if (paragraph->minimum_width > text.minimum_width)
			text.minimum_width=paragraph->minimum_width;
		paragraph->first_char_n=first_char_n;

		first_char_n += paragraph->num_chars;
		if (paragraph->maximum_width_if_one_line
		    > text.real_maximum_width)
			text.real_maximum_width=
				paragraph->maximum_width_if_one_line;

		if (paragraph->height_of_tallest_fragment
		    > text.height_of_tallest_fragment)
			text.height_of_tallest_fragment=
				paragraph->height_of_tallest_fragment;
	}
	auto b=text.paragraphs.begin(), e=text.paragraphs.end();

	if (b != e)
	{
		text.real_width= (*--e)->width;
		text.above_baseline=dim_t::value_type((*e)->above_baseline);
		text.below_baseline=dim_t::value_type((*e)->below_baseline);

		while (b != e)
		{
			if (text.real_width < (*b)->width)
				text.real_width=(*b)->width;
			text.above_baseline += dim_t::value_type((*b)->above_baseline);
			text.above_baseline += dim_t::value_type((*b)->below_baseline);
			++b;
		}
	}
}

bool paragraph_list::rewrap(dim_t width)
{
	bool changed=false;

	richtext_insert_results ignored;

	for (const auto &paragraph:text.paragraphs)
		if (paragraph->rewrap(*this, width, ignored))
			changed=true;

	return changed;
}

bool paragraph_list::unwrap()
{
	bool changed=false;

	for (const auto &paragraph:text.paragraphs)
		if (paragraph->unwrap(*this))
			changed=true;

	if (changed)
		size_changed=true;

	return changed;
}

void paragraph_list::theme_updated(ONLY IN_THREAD,
				   const const_defaulttheme &new_theme)
{
	size_t first_fragment_y_position=0;

	text.paragraphs.for_paragraphs
		(0,
		 [&, this]
		 (const richtextparagraph &p)
		 {
			 p->first_fragment_y_position=
				 first_fragment_y_position;

			 {
				 fragment_list fragments{*this, *p};

				 fragments.theme_was_updated(IN_THREAD,
							     new_theme);
			 }

			 p->first_fragment_y_position=
				 first_fragment_y_position;

			 first_fragment_y_position=
				 p->next_paragraph_y_position();
			 return true;
		 });

	size_changed=true;
}

void paragraph_list::shrink_to_fit()
{
	text.paragraphs.for_paragraphs
		(0,
		 []
		 (const richtextparagraph &p)
		 {
			 p->fragments.for_fragments
				 ([]
				  (const auto &f)
				 {
					 f->string.shrink_to_fit();
				 });
			 return true;
		 });
}

LIBCXXW_NAMESPACE_END
