/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "fonts/current_fontcollection.H"
#include "fonts/freetypefont.H"
#include "richtext/richtext_impl.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextcursorlocation.H"
#include "x/w/screen.H"
#include "messages.H"
#include "assert_or_throw.H"
#include "fragment_list.H"
#include "paragraph_list.H"

#include <x/vector.H>
#include <x/sentry.H>
#include <courier-unicode.h>
#include <algorithm>

LIBCXXW_NAMESPACE_START

richtextObj::implObj::implObj(richtextstring &string,
			      halign alignmentArg)
	: alignment(alignmentArg)
{
	do_set(string);
}

void richtextObj::implObj::set(IN_THREAD_ONLY, richtextstring &string)
{
	// If the existing rich text object has any cursor locations, make
	// a copy of them.

	richtextfragmentObj::locations_t all_locations;

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

	auto restore_paragraphs_sentry=
		make_sentry([&, this]
			    {
				    paragraphs=paragraphs_cpy;
			    });

	restore_paragraphs_sentry.guard();

	do_set(string);

	initialized=false;
	finish_initialization(IN_THREAD);

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

		auto fragment_ptr=&*last_fragment;
		size_t n=fragment_ptr->string.get_string().size()-1;

		for (auto b=fragment_ptr->locations.begin(),
			     e=fragment_ptr->locations.end(); b != e; ++b)
		{
			(*b)->initialize(fragment_ptr, n, b);
		}
	}

	restore_paragraphs_sentry.unguard();
}

void richtextObj::implObj::do_set(richtextstring &string)
{
	paragraphs.clear();
	num_chars=0;

	// Calculate mandatory line breaks.

	std::vector<short> breaks;

	{
		const auto &s=string.get_string();

		breaks.reserve(s.size());

		typedef unicode::linebreak_iter<std::u32string::const_iterator>
			iter_t;

		std::copy(iter_t(s.begin(), s.end()),
			  iter_t(),
			  std::back_insert_iterator<std::vector<short>>
			  (breaks));

		assert_or_throw(breaks.size() == s.size(),
				"Internal error, unicode linebreak marker size not equal to unicode text string size");
	}

	// Each mandatory line break starts a new paragraph.
	// Put each paragraph into a single fragment.

	size_t i=0, n=breaks.size();

	paragraph_list my_paragraphs(*this);

	while (i<n)
	{
		// Find the next mandatory line break.
		size_t j=std::find(&breaks[i+1], &breaks[n],
				   UNICODE_LB_MANDATORY)-&breaks[0];

		auto new_paragraph=my_paragraphs.append_new_paragraph();

		auto new_fragment=
			richtextfragment::create(string,
						 i,
						 j-i,
						 breaks.begin()+i,
						 breaks.begin()+j);


		fragment_list my_fragments(my_paragraphs, *new_paragraph);

		my_fragments.append_no_recalculate(new_fragment);

		i=j;
	}
}

void richtextObj::implObj::finish_initialization(IN_THREAD_ONLY)
{
	if (initialized)
		return;

	paragraph_list my_paragraphs(*this);

	paragraphs.for_paragraphs
		(0,
		 [&]
		 (const auto &new_paragraph)
		 {
			 fragment_list my_fragments(my_paragraphs,
						    *new_paragraph);

			 if (my_fragments.size() != 1)
				 throw EXCEPTION("Internal error: expected 1 fragment in finish_initialization()");

			 auto new_fragment=
				 new_paragraph->get_fragment(0);

			 new_fragment->finish_setting(IN_THREAD);

			 // Now that the fragment has been initialized, the
			 // paragraph's metrics can be recalculated
			 my_fragments.fragment_text_changed(IN_THREAD, 0,
							    0);

			 return true;
		 });
	initialized=true;
}

richtextObj::implObj::~implObj()=default;

void richtextObj::implObj::rich_text_paragraph_out_of_bounds()
{
	throw EXCEPTION("Internal error: rich text paragraph out of bounds.");
}

bool richtextObj::implObj::rewrap(IN_THREAD_ONLY, dim_t width)
{
	paragraph_list my_paragraphs(*this);

	return my_paragraphs.rewrap(IN_THREAD, width);
}

bool richtextObj::implObj::unwrap(IN_THREAD_ONLY)
{
	paragraph_list my_paragraphs(*this);

	return my_paragraphs.unwrap(IN_THREAD);
}

void richtextObj::implObj::insert(IN_THREAD_ONLY,
			 size_t pos,
			 const richtextstring &text)
{
	assert_or_throw(!paragraphs.empty(),
			"Internal error: no paragraphs in insert()");

	// Which paragraph?

	auto paragraph=paragraphs.get_paragraph(find_paragraph_for_pos(pos));

	// Let paragraph_list take care of this, from now on.

	paragraph_list my_paragraphs(*this);

	(*paragraph)->insert(IN_THREAD, my_paragraphs, pos, text);
}

size_t richtextObj::implObj::find_paragraph_for_pos(size_t &pos)
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

richtextparagraph richtextObj::implObj::paragraphs_t::find_paragraph_for_pos(size_t n)
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

richtextfragmentptr richtextObj::implObj::find_fragment_for_y_position(size_t y_pos)
{
	return paragraphs.find_fragment_for_y_position(y_pos);
}

richtextfragmentptr
richtextObj::implObj::paragraphs_t::find_fragment_for_y_position(size_t y_pos)
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

richtextstring richtextObj::implObj::get_as_richtext() const
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
						  .get_string().size();
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

void richtextObj::implObj::theme_updated(IN_THREAD_ONLY)
{
	paragraph_list my_paragraphs(*this);

	my_paragraphs.theme_updated(IN_THREAD);
}

LIBCXXW_NAMESPACE_END