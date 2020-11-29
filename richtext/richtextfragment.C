/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextparagraph.H"
#include "x/w/impl/richtext/richtext.H"
#include "richtext/richtext_insert.H"
#include "richtext/fragment_list.H"
#include "richtext/paragraph_list.H"
#include "richtext/richtext_linebreak_info.H"
#include "assert_or_throw.H"
#include <x/sentry.H>
#include <cmath>
#include <courier-unicode.h>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::richtextfragmentObj);

LIBCXXW_NAMESPACE_START

// Given a font height, compute how big the underline would be. A tiny one
// line for a huge font doesn't look good.

dim_t underline_size(dim_squared_t font_height)
{
	font_height = (font_height + 10) / 20;

	if (font_height == 0)
		font_height=1;

	return dim_t::truncate(font_height);
}


// Even though this text range may not be underlined
// always take into account the underline's
// requirements. See underline().

dim_t adjust_descender_for_underline(dim_t ascender, dim_t descender)
{
	auto n=underline_size(ascender+descender);

	// Underline is positioned n rows
	// below the baseline, so the
	// total space required is n*2

	if (descender < n*2)
		descender=n*2;

	return descender;
}

richtextfragmentObj::richtextfragmentObj()
{
}

richtextfragmentObj::richtextfragmentObj(richtextstring &&string)
	: string{std::move(string)},
	  breaks{string.size(), unicode_lb::none}
{
}

richtextfragmentObj
::richtextfragmentObj(const richtextfragmentObj &current_fragment,
		      size_t substr_pos,
		      size_t substr_len)
	: string(current_fragment.string, substr_pos, substr_len),
	  breaks(current_fragment.breaks.begin()+substr_pos,
		 current_fragment.breaks.begin()+substr_pos+substr_len),
	  horiz_info(current_fragment.horiz_info, substr_pos, substr_len)
{
}



richtextfragmentObj::~richtextfragmentObj()
{
	for (const auto &location:locations)
		location->my_fragment=nullptr;
}


void richtextfragmentObj::get_fragment_out_of_bounds()
{
	throw EXCEPTION("Internal error: rich text fragment out of bounds.");
}

size_t richtextfragmentObj::y_position() const
{
	USING_MY_PARAGRAPH();

	return my_paragraph->first_fragment_y_position+y_pos;
}

std::pair<richtextfragmentObj *, bool>
richtextfragmentObj::find_y_position(size_t y_position_requested)
{
	std::pair<richtextfragmentObj *, bool> ret;
	auto y_position_value=y_position();
	auto my_fragment=this;

	ret.second=false;
	while (y_position_requested < y_position_value)
	{
		ret.second=true;
		auto p=my_fragment->prev_fragment();

		if (!p) break; // ???
		my_fragment=p;
		y_position_value -= dim_t::value_type(my_fragment->height());
	}

	if (ret.second)
	{
		ret.first=my_fragment;
		return ret;
	}

	auto height=my_fragment->height();

	while (y_position_value + dim_t::value_type(height)
	       <= y_position_requested)
	{
		ret.second=true;
		auto p=my_fragment->next_fragment();

		if (!p) break; // ...

		my_fragment=p;
		y_position_value += dim_t::value_type(height);
		height=my_fragment->height();
	}
	ret.first=my_fragment;
	return ret;
}

void richtextfragmentObj
::theme_updated_called_by_fragment_list(ONLY IN_THREAD,
					const const_defaulttheme &new_theme)
{
	string.theme_updated(IN_THREAD, new_theme);
	load_glyphs_widths_kernings();
	recalculate_size_called_by_fragment_list();
	redraw_needed=true;
}

void richtextfragmentObj::load_glyphs_widths_kernings()
{
	USING_MY_PARAGRAPH();

	auto pf=prev_fragment();
	auto nf=next_fragment();

	horiz_info.update
		([&, this]
		 (auto &widths,
		  auto &kernings)
		 {
			 string.compute_width((pf ? &pf->string:NULL),
					      (nf ? &nf->string:NULL),
					      my_paragraph->my_richtext
					      ->paragraph_embedding_level,
					      my_paragraph->my_richtext
					      ->unprintable_char,
					      widths,
					      kernings);
			  });
}

void richtextfragmentObj::update_glyphs_widths_kernings(size_t pos,
							size_t count)
{
	USING_MY_PARAGRAPH();

	auto previous_fragment=prev_fragment();
	auto next_fragment_=next_fragment();

	horiz_info.update
		([&, this]
		 (auto &widths,
		  auto &kernings)
		 {
			 string.compute_width((previous_fragment ?
					       &previous_fragment->string:NULL),
					      (next_fragment_ ?
					       &next_fragment_->string:NULL),
					      my_paragraph->my_richtext
					      ->paragraph_embedding_level,
					      my_paragraph->my_richtext
					      ->unprintable_char,
					      widths,
					      kernings,
					      pos, count);
			  });
}

richtextfragmentObj *richtextfragmentObj::prev_fragment() const
{
	USING_MY_PARAGRAPH();

	size_t n=my_fragment_number;

	if (n > 0)
		return &*my_paragraph->get_fragment(--n);

	auto paragraph_number=my_paragraph->my_paragraph_number;

	if (paragraph_number > 0)
	{
		auto &pr=**my_paragraph->my_richtext->paragraphs
			.get_paragraph(paragraph_number-1);

		assert_or_throw(!pr.fragments.empty(),
				"Internal error: paragraph with no fragments in prev_fragment()");

		auto last=pr.fragments.get_iter(pr.fragments.size()-1);

		return &**last;
	}

	return nullptr;
}

richtextfragmentObj *richtextfragmentObj::next_fragment() const
{
	USING_MY_PARAGRAPH();

	size_t n=my_fragment_number;

	if (++n < my_paragraph->fragments.size())
		return &*my_paragraph->get_fragment(n);

	auto paragraph_number=my_paragraph->my_paragraph_number;

	if (++paragraph_number < my_paragraph->my_richtext->paragraphs.size())
	{
		auto &pr=**my_paragraph->my_richtext->paragraphs
			.get_paragraph(paragraph_number);

		assert_or_throw(!pr.fragments.empty(),
				"Internal error: paragraph with no fragments in next_fragment()");

		auto first=pr.fragments.get_iter(0);

		return &**first;
	}

	return nullptr;
}

richtextfragmentObj::wrap_t richtextfragmentObj::wrap_right_fragment_and_pos()
{
	USING_MY_PARAGRAPH();

	size_t l=string.size();
	assert_or_throw(l > 0, "empty fragment in wrap_right_fragment_and_pos");

	// If we have no place to go off the right side of the fragment,
	// remain at the last character.
	wrap_t ret{ref{this}, --l};

	if (string.embedding_level(my_paragraph->my_richtext
				   ->paragraph_embedding_level)
	    == UNICODE_BIDI_LR)
	{
		// From the right end of the fragment we go to the next line.
		richtextfragmentObj *f=next_fragment();

		if (f)
			f->first_wrapped_pos(ret);
	}
	else
	{
		// From the right end of the fragment we go to the previous
		// line.
		richtextfragmentObj *f=prev_fragment();

		if (f)
			f->last_wrapped_pos(ret);
	}

	return ret;
}

richtextfragmentObj::wrap_t richtextfragmentObj::wrap_left_fragment_and_pos()
{
	USING_MY_PARAGRAPH();

	size_t l=string.size();
	assert_or_throw(l > 0, "empty fragment in wrap_left_fragment_and_pos");

	// If we have no place to go off the left side of the fragment,
	// remain at the first character.

	wrap_t ret{ref{this}, 0};

	if (string.embedding_level(my_paragraph->my_richtext
				   ->paragraph_embedding_level)
	    == UNICODE_BIDI_LR)
	{
		// From the left end of the fragment we go to the previous
		// line.

		richtextfragmentObj *f=prev_fragment();

		if (f)
			f->last_wrapped_pos(ret);
	}
	else
	{
		// From the left end of the fragment we go to the next line.

		richtextfragmentObj *f=next_fragment();

		if (f)
			f->first_wrapped_pos(ret);
	}

	return ret;
}

void richtextfragmentObj::first_wrapped_pos(wrap_t &ret)
{
	USING_MY_PARAGRAPH();

	size_t l=string.size();
	assert_or_throw(l > 0, "empty fragment in first_wrapped_pos");

	std::get<0>(ret)=ref{this};

	if (string.embedding_level(my_paragraph->my_richtext
				   ->paragraph_embedding_level)
	    == UNICODE_BIDI_LR)
	{
		// We wrapped to the beginning of this line.

		std::get<1>(ret)=0;
	}
	else
	{
		// We wrapped to the end of this line.
		std::get<1>(ret)=l-1;
	}
}

void richtextfragmentObj::last_wrapped_pos(wrap_t &ret)
{
	USING_MY_PARAGRAPH();

	size_t l=string.size();
	assert_or_throw(l > 0, "empty fragment in last_wrapped_pos");

	std::get<0>(ret)=ref{this};

	if (string.embedding_level(my_paragraph->my_richtext
				   ->paragraph_embedding_level)
	    == UNICODE_BIDI_LR)
	{
		// We wrapped to the end of this line.
		std::get<1>(ret)=l-1;
	}
	else
	{
		// We wrapped to the beginning of this line.
		std::get<1>(ret)=0;
	}
}

void richtextfragmentObj::recalculate_size_called_by_fragment_list()
{
	width=0;
	minimum_width=0;
	above_baseline=0;
	below_baseline=0;

	assert_or_throw(horiz_info.size() == breaks.size(),
			"Internal error: different width and breaks/kernings vectors");

	// Loop over the text string backwards, adding up the width and
	// kerning of each character (with the exceptions noted) to compute
	// the total width of the line. Also leverage this loop to compute
	// the minimum width of this line. This is used, to set the minimum
	// size of the rich text, which is the maximum minimum.

	dim_t width_so_far=0;

	initial_width_lr=0;

	// If the fragment starts with rl text, find where the lr text starts.
	size_t lr_starts;

	{
		const auto &meta=string.get_meta();

		auto b=meta.begin(), e=meta.end();

		while (b != e && b->second.rl)
		{
			++b;
		}

		lr_starts= b == e ? string.size():b->first;
	}

	std::optional<dim_t> first_rl_width;

	for (size_t i=horiz_info.size(); i; )
	{
		--i;

		// Width of this character, together with the width of this
		// character plus its kerning from the previous character.

		auto c_width=horiz_info.width(i);
		auto c_total=c_width+horiz_info.kerning(i);

		// The kerning is not used for the first character.

		if (i == 0)
		{
			width = dim_t::truncate(width + c_width);
		}
		else
		{
			width = dim_t::truncate(width + c_total);
		}

		// We're seeing something in the line that can break here?

		if (i == 0 || breaks[i] != unicode_lb::none)
		{
			width_so_far =
				dim_t::truncate(width_so_far + c_width);

			if (width_so_far > minimum_width)
				minimum_width=width_so_far;

			// This loop iterates backwards. The last time we
			// get here we will compute the width of the
			// initial non-breakable sequence
			initial_width_lr=width_so_far;

			// However if there's initial rl text, we want to
			// grab the first non-breakable sequence we see,
			// when we reach it (lr_starts is 0 if there are no
			// leading rl sections).
			if (i < lr_starts && !first_rl_width)
				first_rl_width=width_so_far;

			// Reset for the next one.
			width_so_far=0;
		}
		else
		{
			// Otherwize, the minimum width includes the kerning.
			width_so_far =
				dim_t::truncate(width_so_far + c_total);
		}
	}

	// Edge case: initial rl sequence.
	if (first_rl_width)
		initial_width_lr=*first_rl_width;

	initial_width_rl_trailing_lr=0;
	initial_width_rl=0;

	{
		const auto &meta=string.get_meta();

		auto b=meta.begin(), e=meta.end();

		while (b != e && !e[-1].second.rl)
		{
			--e;
		}

		size_t n=string.size();

		lr_starts= e==meta.end() ? n:e->first;

		if (lr_starts < n)
		{
			bool found_break=false;

			while (lr_starts < n)
			{
				auto total_width=
					dim_t::truncate(horiz_info.width
							(lr_starts) +
							horiz_info.kerning
							(lr_starts));

				initial_width_rl_trailing_lr += total_width;

				if (found_break)
				{
					++lr_starts;
					continue;
				}
				initial_width_rl += total_width;

				++lr_starts;

				if (lr_starts < n &&
				    breaks[lr_starts] != unicode_lb::none)
					found_break=true;
			}
		}
		else
		{
			auto &s=string.get_string();

			while (n > 0)
			{
				--n;
				if (s[n] == '\n')
					break; // n must be 0

				initial_width_rl +=
					dim_t::truncate(horiz_info.width(n));

				if (breaks[n] != unicode_lb::none)
					break;
				if (n > 0 && s[n-1] == '\n') // Ditto
					break;

				initial_width_rl +=
					dim_t::truncate(horiz_info.kerning(n));
			}
			initial_width_rl_trailing_lr=initial_width_rl;
		}
	}

	const auto &resolved_fonts=RESOLVE_FONTS();

	// Now go through the resolved fonts, determining the biggest ascender
	// and descender, to compute this fragment's ascender and descender.

	for (const auto &info:resolved_fonts)
	{
		const freetypefont &render_font=info.second;

		auto ascender=render_font->ascender;
		auto descender=adjust_descender_for_underline
			(render_font->ascender,
			 render_font->descender);

		if (above_baseline < ascender)
			above_baseline=ascender;
		if (below_baseline < descender)
			below_baseline=descender;
	}

	for (const auto &location:locations)
		location->horiz_pos_no_longer_valid();
}

void
richtextfragmentObj::insert_new_paragraphs(ONLY IN_THREAD,
					   paragraph_list &my_paragraphs,
					   create_fragments_from_inserted_text
					   &factory,
					   const richtextfragment
					   &insert_before,
					   richtext_insert_results &results)
{
	// TODO -- figure out how to optimize insertions of multiple
	// paragraphs.

	while (1)
	{
		auto next_line=factory.next_string();

		if (next_line.size() == 0)
		{
			results.insert_ended_in_paragraph_break=true;
			break; // Inserted text ended in a paragraph break.
		}
		auto &next_line_s=next_line.get_string();

		// If next_line does not have a new line this must be
		// the chunk of text after the last paragraph break. The
		// newline can appear only in the first or the last position.

		auto seen_paragraph_break=
			next_line_s.at(next_line.size()-1) == '\n' ||
			next_line_s.at(0) == '\n';

		if (!seen_paragraph_break)
		{
			// Must be the last one.

			size_t insert_pos=0;

			if (my_paragraphs.text.rl())
				insert_pos=insert_before->string.size();

			insert_before->insert(IN_THREAD, my_paragraphs,
					      insert_pos,
					      std::move(next_line),
					      results);
			break;
		}

		// We create a new paragraph here, consisting of a single
		// fragment, our next_line.

		auto new_paragraph=my_paragraphs
			.insert_new_paragraph(insert_before->my_paragraph
					      ->my_paragraph_number);

		fragment_list my_fragments{my_paragraphs,
			*new_paragraph};

		size_t n=next_line.size();

		results.inserted_range.try_emplace
			(my_fragments.append_new_fragment(std::move(next_line)))
			.first->second.add(richtext_insert_results::range
					   {0, n});
	}
}

void richtextfragmentObj
::split_after_me(paragraph_list &my_paragraphs)
{
	auto n=next_fragment();

	assert_or_throw(!n || n->my_fragment_number > 0,
			"split_after_me called for a last fragment");

	auto new_paragraph=my_paragraphs
		.insert_new_paragraph(my_paragraph->my_paragraph_number+1);
	fragment_list new_paragraph_fragments{my_paragraphs,
		*new_paragraph};

	new_paragraph_fragments.split_from(this);
}

richtext_insert_results
richtextfragmentObj::insert(ONLY IN_THREAD,
			    paragraph_list &my_paragraphs,
			    const richtext_insert_base &new_string_to_insert)
{
	USING_MY_PARAGRAPH();

	// Ok, get the position and the contents to insert.
	auto pos=new_string_to_insert.pos();
	auto new_string=new_string_to_insert(string.meta_at(pos));

	richtext_insert_results results;

	// Find all locations at the insert position.
	for (const auto &l:locations)
	{
		if (l->get_offset() == pos)
			results.cursors_to_move.push_back(l);
	}

	auto &my_string=string.get_string();

	assert_or_throw(pos < my_string.size(),
			"insert position after the fragment");

	// Assume there's at least one paragraph break in the string
	//
	// Use create_fragments_from_inserted_text to generate one fragment
	// for each individual paragraph.

	create_fragments_from_inserted_text factory{new_string,
		my_paragraphs.text.paragraph_embedding_level};

	auto richtext_rl=my_paragraphs.text.rl();

	// Whether the first paragraph (or the only text) is right to left
	// text. This is determined by next_string_dir(), but if it's
	// richtext_dir::both, or if the insert position is a paragraph
	// break, this gets overriden to our paragraph embedding level.

	auto insert_rl=richtext_rl;

	if (my_string[pos] != '\n' && new_string.size() > 0)
	{
		switch (factory.next_string_dir()) {
		case richtext_dir::lr:
			insert_rl=false;
			break;
		case richtext_dir::rl:
			insert_rl=true;
			break;
		case richtext_dir::both:
			break;
		}
	}

#if 0
	results.insert_rl=insert_rl;
#endif
	if (insert_rl)
		++pos; // Insertion position *after* what's indicated.

	// Sanity check, non-empty string and ...
	assert_or_throw(my_string.empty() ||
			(
			 // lr text: can't insert after trailing \n
			 !(!richtext_rl && *--my_string.end() == '\n' &&
			   pos == my_string.size())
			 &&
			 // rl text: can't insert before leading \n
			 !(richtext_rl && my_string[0] == '\n' && pos == 0)
			 ),
			"insert position cannot be after the paragraph break");

	if (auto &s=new_string.get_string(); std::find(s.begin(), s.end(), '\n')
	    == s.end())
	{
		// Easy case, no paragraph breaks to worry about.

		insert(IN_THREAD, my_paragraphs, pos,
		       std::move(new_string), results);

		return results;
	}

	if (pos == my_string.size())
	{
		auto this_embedding_level=
			string.embedding_level(my_paragraphs.text
					       .paragraph_embedding_level);
		// Must be appending right to left, must be insert_rl
		//
		//
		// What we'll do is insert the first paragraph at the end
		// of the fragment, then split it off.

		insert(IN_THREAD, my_paragraphs, pos,
		       factory.next_string(), results);

		if (this_embedding_level != UNICODE_BIDI_LR)
		{
			// No possible locations can be located here, all
			// locations would be at the end of the current string.
			//
			// Find all locations at the end of the string which
			// are do_not_adjust_on_insert, and manually put them
			// at the end of the first character of the insert
			// text, from the right, the logically equivalent
			// position after insert, for the before-inserted
			// position.
			//
			// Original fragment:  ttttttt
			// Position:                 ^
			//
			// We appended nnnnnn
			//
			// New fragment:       tttttttnnnnnn
			// New position:                   ^

			fragment_list my_fragments{my_paragraphs,
				*my_paragraph};

			// This is right to left embedding level, so we just
			// inserted a newline at the insert position. Do a
			// right-to-left split, to split the preceding content
			// into the next line.
			if (richtext_rl)
			{
				split(my_fragments, pos, split_rl, true,
				      results);
			}
		}

		split_after_me(my_paragraphs);

		insert_new_paragraphs(IN_THREAD,
				      my_paragraphs,
				      factory,
				      ref{next_fragment()},
				      results);

		return results;
	}

	if (!richtext_rl)
	{
		// For inserting left-to-right paragraphs, we start the
		// ball rolling to split the fragment where text is getting
		// inserted at the insert position, then set insert_before
		// to the split-off fragment. Each new fragment that gets
		// inserted will get inserted before that one.
		//
		// Text:             aaaaaa|bbbbbb
		//
		// "|" marks the insert position. This gets split:
		//
		//                   aaaaaa
		// insert_before->   bbbbbb
		//
		// If, however, nothing gets split and insert_before will be
		// this fragment.

		richtextfragment insert_before{this};

		if (pos > 0)
		{
			fragment_list my_fragments{my_paragraphs,
				*my_paragraph};

			split(my_fragments, pos, split_lr, true, results);

			auto p=next_fragment();
			assert_or_throw(p,
					"could not find split fragment");
			insert_before=p;
		}

		// The split-off fragment, "bbbbbb", will be where we'll
		// be inserting everything else, before it.
		//
		// We are going to be inserting at least one paragraph.
		//
		// insert_before, therefore, should start a new paragraph,
		// so we move it to a new paragraph, first. Then, we'll
		// take the first paragraph from the inserted text and
		// append it to the previous paragraph, that contains
		// "aaaaaa".

		if (insert_before->my_fragment_number > 0)
		{
			// Now take the first paragraph to insert and add
			// it to "aaaaaa", after splitting off bbbbbb.


			split_after_me(my_paragraphs);
			auto next_line=factory.next_string();

			assert_or_throw(next_line.get_string().at
					(next_line.size()-1) == '\n',
					"expected at least one"
					" paragraph");

			insert(IN_THREAD, my_paragraphs, string.size(),
			       std::move(next_line), results);
		}

		insert_new_paragraphs(IN_THREAD,
				      my_paragraphs,
				      factory,
				      insert_before,
				      results);
	}
	else
	{
		// We get here only when pos < my_string.size()
		//
		// Text:             aaaaaa|bbbbbb
		//
		// "|" marks the insert position, pos. This gets split:
		//
		//                   bbbbbb
		// insert_before->   aaaaaa

		richtextfragmentObj *p;

		{
			fragment_list my_fragments{my_paragraphs,
				*my_paragraph};

			split(my_fragments, pos, split_rl, true, results);

			p=next_fragment();
			assert_or_throw(p,
					"could not find split fragment");
		}
		richtextfragment insert_before{p};

		// The split-off fragment, "aaaaaa", will be where we'll
		// be inserting everything else, before it.
		//
		// We are going to be inserting at least one paragraph.
		//
		// insert_before, therefore, should start a new paragraph,
		// so we move it to a new paragraph, first.

		split_after_me(my_paragraphs);
		auto next_line=factory.next_string();

		assert_or_throw(next_line.get_string().at(0) == '\n',
				"expected at least one"
				" paragraph");

		insert(IN_THREAD, my_paragraphs, 0,
		       std::move(next_line), results);

		insert_new_paragraphs(IN_THREAD,
				      my_paragraphs,
				      factory,
				      insert_before,
				      results);
	}
	return results;
}

void richtextfragmentObj::insert(ONLY IN_THREAD,
				 paragraph_list &my_paragraphs,
				 size_t pos,
				 richtextstring &&new_string,
				 richtext_insert_results &insert_results)
{
	// Sanity checks
	assert_or_throw(pos <= string.size(),
			"Invalid pos parameter to insert()");

	auto n_size=new_string.size();

	if (n_size == 0) return;

	// Make sure things will unwind properly, in the event of an unlikely
	// exception.

	auto string_sentry=
		make_sentry([&]
			    {
				    string.erase(pos, n_size);
			    });

	auto breaks_sentry=
		make_sentry([&]
			    {
				    breaks.erase(breaks.begin()+pos,
						 breaks.begin()+(pos+n_size));
			    });

	auto horiz_info_sentry=
		make_sentry([&]
			    {
				    horiz_info.erase(pos, pos+n_size);
			    });

	string.insert(pos, std::move(new_string));
	string_sentry.guard();

	// Make room for breaks, widths, and kernings.

	breaks.insert(breaks.begin()+pos, n_size, unicode_lb::none);
	breaks_sentry.guard();

	horiz_info.insert(pos, n_size);
	horiz_info_sentry.guard();
#if 0
	no_meta_for_trailing_space();
#endif

	update_glyphs_widths_kernings(pos, n_size);

	recalculate_linebreaks();

	fragment_list my_fragments{my_paragraphs, *my_paragraph};

	auto old_width=width;

	my_fragments.fragment_text_changed(my_fragment_number,
					   n_size);

	assert_or_throw(width >= old_width,
			"Internal error: inserting characters reduced text fragment width?");

	// Update cursor locations

	for (const auto &location:locations)
		location->inserted_at(IN_THREAD,
				      pos, n_size,
				      width-old_width);

	// At this point, it's new paragraphs, or bust!
	string_sentry.unguard();
	breaks_sentry.unguard();
	horiz_info_sentry.unguard();

	redraw_needed=true;

	insert_results.inserted_range[ref{this}]
		.add(richtext_insert_results::range{pos, pos+n_size});
}

// Recalculate line breaks for the previous fragment, this one,
// and the next fragment. We begin with the previous fragment,
// count the number of line breaks values to skip, how many line
// break values to count, then run all fragments through the
// linebreaking algorithm.

void richtextfragmentObj::recalculate_linebreaks()
{
	USING_MY_PARAGRAPH();

	const std::u32string &current_string=string.get_string();

	size_t skip=0;

	auto start_with=my_fragment_number;
	size_t n=1;

	if (start_with > 0)
	{
		--start_with;
		++n;
		skip += my_paragraph->get_fragment(start_with)->string.size();
	}

	auto end_with=my_fragment_number;

	if (++end_with < my_paragraph->fragments.size())
		++n;


	richtextstring *ptrs[n];

	for (size_t i=0; i<n; ++i)
	{
		ptrs[i]=&my_paragraph->get_fragment(start_with)->string;
		++start_with;
	}

	richtext_linebreak_info(skip, current_string.size(), &breaks[0],
				ptrs, n);

	start_with=my_fragment_number;
	if (start_with == 0)
	{
		// If this is the first fragment in the paragraph, and this
		// is not the first paragraph, the first character's line
		// breaking value must be unicode_lb::mandatory
		//
		// If this is the first fragment in paragraph 0, it's
		// uncode_lb::none (this logic is also present in do_set).

		breaks[0]=my_paragraph->my_paragraph_number > 0
			? unicode_lb::mandatory:unicode_lb::none;
	}
}

#if 0
richtextmetamap_t::iterator richtextfragmentObj::find_meta_for_pos(size_t pos)
{
	auto meta_iter=metadata.upper_bound(pos);

	assert_or_throw(meta_iter != metadata.begin(),
			"Internal error: upper_bound(pos) returned begin() in find_meta_for_pos().");

	return --meta_iter;
}

size_t richtextfragmentObj::meta_ending_pos(richtextmetamap_t::iterator iter)
{
	assert_or_throw(iter != metadata.end(),
			"Internal error: meta_ending_pos received ending iterator value");

	if (++iter == metadata.end())
		return string.size();

	return iter->first;
}

void richtextfragmentObj::no_meta_for_trailing_space()
{

	const std::u32string &current_string=string.get_string();

	if (next_fragment())
		return;

	auto last_location=current_string.size()-1;

	auto iter=find_meta_for_pos(last_location);

	if (iter->first == last_location)
		metadata.erase(iter);
}
#endif

coord_t richtextfragmentObj::first_xpos(ONLY IN_THREAD) const
{
	assert_or_throw(my_paragraph && my_paragraph->my_richtext,
			"Internal error: fragment not linked.");

	auto richtext=my_paragraph->my_richtext;
	auto text_width=richtext->width();

	if (richtext->text_width)
		text_width=richtext->text_width.value();

	if (width < text_width)
	{
		auto pad=dim_t::value_type(text_width-width);
		auto alignment=richtext->alignment;

		if (alignment == halign::center)
			return coord_t::truncate(pad/2);

		if (alignment == halign::right)
			return coord_t::truncate(pad);
	}
	else
	{
		auto extra=-coord_squared_t::truncate(width - text_width);
		auto alignment=richtext->alignment;

		if (alignment == halign::center)
			return coord_t::truncate(extra/2);

		if (alignment == halign::right)
			return coord_t::truncate(extra);
	}
	return 0;
}

dim_t richtextfragmentObj::x_width(ONLY IN_THREAD)
{
	assert_or_throw(my_paragraph && my_paragraph->my_richtext,
			"Internal error: fragment not linked.");

	return horiz_info.width();
}

////////////////////////////////////////////////////////////////////////////

void richtextfragmentObj::split(fragment_list &my_fragments, size_t pos,
				enum split_t type, bool force,
				richtext_insert_results &insert_results)
{
	USING_MY_PARAGRAPH();

	const std::u32string &current_string=string.get_string();

	assert_or_throw(pos > 0 && pos < current_string.size() &&
			(force ||
			 breaks[pos] != unicode_lb::none),
			"Internal error: attempting to split a text fragment at a disallowed position");


	assert_or_throw(my_fragments.paragraph.my_paragraph_number ==
			my_paragraph->my_paragraph_number,
			"Internal error: wrong fragments parameter to split()");

	auto break_type=breaks[pos];

	// We can copy the relevant parts from myself, into a new fragment...
	auto new_fragment=type == split_rl
		? richtextfragment::create(*this,
					   0,
					   pos)
		: richtextfragment::create(*this,
					   pos,
					   current_string.size()-pos);

	// ... then truncate myself accordingly.

	if (type == split_rl)
	{
		string.erase(0, pos);
		breaks.erase(breaks.begin(), breaks.begin()+pos);
		horiz_info.erase(0, pos);

		insert_results.split(ref{this}, 0, pos, new_fragment);
	}
	else
	{
		size_t n_erased=current_string.size()-pos;
		string.erase(pos, n_erased);
		breaks.erase(breaks.begin()+pos, breaks.end());
		horiz_info.erase(pos, n_erased);
		insert_results.split(ref{this}, pos, n_erased, new_fragment);
	}
	// We also must move any cursor locations to the new fragment.

	for (auto iter=locations.begin(); iter != locations.end(); )
	{
		auto p=iter;

		++iter;

		bool was_split_off_from_parent=
			(*p)->get_offset() >= pos;

		bool is_moved_to_new_fragment=false;

		if (was_split_off_from_parent)
		{
			if (type == split_lr)
				is_moved_to_new_fragment=true;
		}
		else
		{
			if (type == split_rl)
				is_moved_to_new_fragment=true;
		}

		if (is_moved_to_new_fragment || was_split_off_from_parent)
		{
			auto l= *p;

			if (is_moved_to_new_fragment)
			{
				move_location(p, new_fragment);
			}
			if (was_split_off_from_parent)
				l->split_from_fragment(pos);
		}
	}

	if (!force && break_type == unicode_lb::mandatory)
	{
		// Copy split content into a new paragraph.

		auto new_paragraph=my_fragments.my_paragraphs
			.insert_new_paragraph(my_paragraph->my_paragraph_number
					      +1);

		// This was split from here.
		fragment_list
			new_paragraph_fragments{my_fragments.my_paragraphs,
						*new_paragraph};

		// Move the remaining fragments to the new paragraph
		new_paragraph_fragments.split_from(new_fragment, this);
	}
	else
	{
		auto iter=my_paragraph->fragments.get_iter(my_fragment_number);

		++iter;

		my_fragments.insert_no_change_in_char_count(iter, new_fragment);
	}

	if (new_fragment->string.size())
		new_fragment->update_glyphs_widths_kernings(0, 1);

	my_fragments.fragment_text_changed(my_fragment_number, 0);

	redraw_needed=true;
}

void richtextfragmentObj::move_location(locations_t::iterator iter,
					const richtextfragment &new_fragment)
{
	auto l=*iter;

	// This cursor location is now in the new
	// fragment

	new_fragment->locations.push_back(l);
	locations.erase(iter);

	l->my_fragment=&*new_fragment;
	l->my_fragment_iter=--new_fragment->locations.end();
}

void richtextfragmentObj::merge(fragment_list &my_fragments,
				merge_type_t merge_type,
				richtext_insert_results &insert_results)
{
	USING_MY_PARAGRAPH();

	assert_or_throw(my_fragments.paragraph.my_paragraph_number ==
			my_paragraph->my_paragraph_number,
			"Internal error: wrong my_fragments parameter to split()");

	auto n=my_fragment_number;

	++n;

	if (n == my_paragraph->fragments.size())
		// Merge next paragraph's fragments?
		my_fragments.join_next();

	assert_or_throw(n != my_fragments.size(),
			"Internal error: merge() called on the last paragraph");

	// Look at the next fragment, and unceremoniously append it to this
	// fragment.

	auto other=my_paragraph->get_fragment(n);

	if (my_paragraph->my_richtext->paragraph_embedding_level ==
	    UNICODE_BIDI_LR)
	{
		if (merge_type == merge_bidi &&
		    string.get_dir() == richtext_dir::rl)
		{
			merge_lr_rl(my_fragments, other, insert_results);
			return;
		}

		merge_lr_lr(my_fragments, other, insert_results);

		if (merge_type == merge_paragraph)
		{
			update_glyphs_widths_kernings(0, 1);

			auto p=prev_fragment();

			if (p)
				p->update_glyphs_widths_kernings(0, 1);
		}
	}
	else
	{
		if (merge_type == merge_bidi &&
		    string.get_dir() == richtext_dir::lr)
		{
			merge_rl_lr(my_fragments, other, insert_results);
			return;
		}

		other->merge_lr_lr(my_fragments, ref{this},
				   insert_results);

		if (merge_type == merge_paragraph)
		{
			other->update_glyphs_widths_kernings(0, 1);

			auto p=other->prev_fragment();

			if (p)
				p->update_glyphs_widths_kernings(0, 1);
		}
	}
}

void richtextfragmentObj::merge_rl_lr(fragment_list &my_fragments,
				      const richtextfragment &other,
				      richtext_insert_results &insert_results)
{
	if (other->string.get_dir() == richtext_dir::lr)
	{
		merge_lr_lr(my_fragments, other, insert_results);
		return;
	}

	auto &meta=other->string.get_meta();
	auto mb=meta.begin();
	auto me=meta.end();

	assert_or_throw(mb != me,
			"Internal error: empty meta in merge_rl_lr");

	auto p=me;

	while (p > mb && !p[-1].second.rl)
		--p;

	if (p == me)
	{
		// Next fragment ends with rl text, regular rl merge.

		other->merge_lr_lr(my_fragments, ref{this}, insert_results);
		return;
	}

	other->split(my_fragments, p->first, split_lr, false,
		     insert_results);

	other->merge_lr_lr(my_fragments, ref{this}, insert_results);

	auto next=other->next_fragment();

	assert_or_throw(next, "Internal error: did not find split fragment in"
			" merge_rl_lr");
	other->merge_lr_lr(my_fragments, ref{next}, insert_results);
}

void richtextfragmentObj::merge_lr_rl(fragment_list &my_fragments,
				      const richtextfragment &other,
				      richtext_insert_results &insert_results)
{

	size_t next_left_to_right_start=
		other->string.left_to_right_start();

	if (next_left_to_right_start == 0)
	{
		merge_lr_lr(my_fragments, other, insert_results);
		return;
	}

	bool merge_again=false;

	// If the following fragment has left-to-right text, split
	// it from it. From this point on, the following fragment has
	// only right to left text.

	if (other->string.get_string().size()
	    > next_left_to_right_start)
	{
		merge_again=true;
		other->split(my_fragments, next_left_to_right_start,
			     split_lr, true, insert_results);
	}

	// Instead, merge this fragment at the end of the next one.
	other->merge_lr_lr(my_fragments, ref{this}, insert_results);

	if (merge_again)
	{
		// left-to-right text must follow, so this won't
		// recurse again.
		other->merge(my_fragments, other->merge_bidi,
			     insert_results);
	}
}

void richtextfragmentObj::merge_lr_lr(fragment_list &my_fragments,
				      const richtextfragment &other,
				      richtext_insert_results &insert_results)
{
	const std::u32string &current_string=string.get_string();

	auto orig_size=current_string.size();

	redraw_needed=true;

	if (!other->string.get_string().empty())
	{
		size_t orig_pos=current_string.size();

		breaks.reserve(breaks.size() + other->breaks.size());

		string.insert(orig_pos, other->string);

		breaks.insert(breaks.end(), other->breaks.begin(),
			      other->breaks.end());

		horiz_info.append(other->horiz_info);

		update_glyphs_widths_kernings(orig_pos, 1);

		insert_results.merged(other, ref{this}, orig_pos);
	}

	// Migrate the cursor locations too
	while (!other->locations.empty())
	{
		auto l=other->locations.front();

		locations.push_back(l);
		l->my_fragment=this;
		l->my_fragment_iter=--locations.end();
		l->merged_from_fragment(orig_size);
		other->locations.pop_front();
		l->split_from_fragment(0);
	}
	my_fragments.erase(my_paragraph->fragments
			   .get_iter(other->my_fragment_number));
	my_fragments.fragment_text_changed(my_fragment_number, 0);
}

void richtextfragmentObj::remove(size_t pos,
				 size_t nchars,
				 fragment_list &my_fragments,
				 richtext_insert_results &results)
{
	if (nchars == 0)
		return;

	USING_MY_PARAGRAPH();

	results.removed(ref{this}, pos, nchars);

	auto &current_string=string.get_string();

	assert_or_throw(pos < current_string.size() &&
			nchars < current_string.size(),
			"invalid starting position or remove length");

	if (my_fragments.my_paragraphs.text.rl())
	{
		assert_or_throw(current_string.size()-pos >= nchars &&
				pos > 0,
				"invalid character rl range in remove()");
	}
	else
	{
		assert_or_throw(current_string.size()-pos > nchars,
				"invalid character lr range in remove()");
	}

	redraw_needed=true;

	// Remove all the bits and bytes

	string.erase(pos, nchars);

	breaks.erase(breaks.begin()+pos, breaks.begin()+pos+nchars);
	horiz_info.erase(pos, nchars);

	update_glyphs_widths_kernings(pos < current_string.size()
				      ? pos:pos-1, 1);

	// Adjust all locations on or after the removal point.
	for (const auto &l:locations)
	{
		l->removed_from_fragment(pos, nchars);
	}

	recalculate_linebreaks();

	my_fragments.fragment_text_changed(my_fragment_number, -nchars);
}

void richtextfragmentObj
::move_locations_to_another_fragment(richtextfragmentObj *n)
{
	for (auto b=locations.begin(), e=locations.end(); b != e; )
	{
		auto p=b;

		++b;

		auto l=*p;

		l->my_fragment_iter=
			n->locations.insert(n->locations.end(), l);
		l->my_fragment=n;

		l->start_of_line();
		locations.erase(p);
	}
}

size_t richtextfragmentObj::index() const
{
	USING_MY_PARAGRAPH();

	size_t n=my_paragraph->first_fragment_n;

	return n + my_fragment_number;
}

///////////////////////////////////////////////////////////////////////////////

#if 0
void richtextfragmentObj::fragments_t
::append_rich_text(std::vector<unicode_char> &text,
		   richtextmetamap_t &meta)
{
	const std::u32string &current_string=string.get_string();

	for (const auto &fragment: static_cast<v_t &>(*this))
	{
		if (fragment->text.empty())
			continue; // Shouldn't happen

		auto p=current_string.size();

		text.insert(text.end(),
			    fragment->text.begin(),
			    fragment->text.end());

		if (meta.empty())
		{
			meta=fragment->metadata;
		}
		else
		{
			richtextmeta::insert(meta, p,
					     fragment->current_string.size(),
					     fragment->metadata);
		}
	}
}
#endif

// A thrown exception can end up nuking some fragments

void richtextfragmentObj::fragments_t::paragraph_destroyed()
{
	for (const auto &fragment: static_cast<v_t &>(*this))
		fragment->my_paragraph=nullptr;
}

richtextfragment richtextfragmentObj::fragments_t::find_fragment_for_pos(size_t &pos) const
{
	auto iter=std::lower_bound(begin(),
				   end(), pos,
				   []
				   (const richtextfragment &f, size_t pos)
				   {
					   return f->first_char_n <= pos;
				   });

	assert_or_throw(iter != begin(),
			"Internal error: empty list in find_fragment_for_pos");

	auto fragment=*--iter;

	pos -= fragment->first_char_n;

	size_t s=fragment->string.size();

	if (pos >= s)
	{
		assert_or_throw(s,
				"Internal error: empty fragment in find_fragment_for_pos()");
		pos=s-1;
	}

	return fragment;
}

dim_t richtextfragmentObj::compute_initial_width_for_bidi() const
{
	// Is the next fragment starts with something small enough
	// to get merged into here?

	auto nextp=next_fragment();

	if (my_paragraph->my_richtext->paragraph_embedding_level ==
	    UNICODE_BIDI_LR)
	{
		auto initial_width=nextp->initial_width_lr;

		// If this fragment is rl, the next fragment's initial_width_lr
		// will work for us, either way. If the next fragment starts
		// with rl text, this is going to be the last rl text segment.
		//
		// If this fragment is not rl, and the next fragment begins
		// with rl text, it must fit.

		if (string.get_dir() != richtext_dir::rl)
		{
			size_t p=nextp->string.left_to_right_start();

			if (p > 0)
			{
				initial_width=p >= nextp->string.size()
					? nextp->horiz_info.width()
					: nextp->horiz_info.x_pos(p);
			}
		}
		return initial_width;
	}
	else
	{
		// If this fragment is lr, the next fragment's initial_width_rl
		// will work for us, either way. If the next fragment ends
		// with lr text, this is going to be the first lr text segment.
		// If the next fragment ends with rl text, this is going to
		// be its last breakable segment.
		if (string.get_dir() == richtext_dir::lr)
			return nextp->initial_width_rl;

		return nextp->initial_width_rl_trailing_lr;
	}
}

size_t richtextfragmentObj::compute_split_pos_lr(dim_t desired_width) const
{
	auto &meta=string.get_meta();
	auto metab=meta.begin();
	auto metae=meta.end();

	// Find the index of the character that reached "desired_width"
	size_t xpos=horiz_info.find_x_pos(desired_width);

	// Find this character's metadata.
	auto p=richtextstring::meta_upper_bound_by_pos(meta, xpos);

	if (p > metab)
		--p;

	// Did we land in the middle of right-to-left text? If so, we
	// won't break inside it.

	if (p->second.rl)
	{
		// Search for the start of the right-to-left text, we'll
		// break there.

		while (1)
		{
			if (p == metab)
			{
				// Right to left text at the beginning of
				// the line. We can't break there. The best
				// we can do is find where left-to-right
				// text starts. This will exceed the requested
				// width. So be it.

				while (1)
				{
					if (p == metae)
					{
						xpos=string.size();
						break;
					}
					if (!p->second.rl)
					{
						xpos=p->first;
						break;
					}
					++p;
				}
				break;
			}
			if (!p[-1].second.rl)
			{
				// Start of right to left text, break here.
				xpos=p->first;
				break;
			}
			--p;
		}
	}
	else
	{
		// We landed in the middle of left-to-right text. Check to
		// see if there's any right to left text before. There /should/
		// be a break there, but this part of the code won't ass-ume it.

		size_t first_lr=0;

		while (p > metab)
		{
			if (p[-1].second.rl)
			{
				first_lr=p->first;
				break;
			}
			--p;
		}

		while (1)
		{
			if (xpos == first_lr)
			{
				// Reached either position 0 or start of
				// right-to-left text. If not position
				// 0 we can break here.
				if (xpos > 0)
					break;

				// Left to right text at the beginning of the
				// line. Find where left-to-right text ends.

				size_t n=string.size();

				for (p=metab; p != metae; ++p)
				{
					if (p->second.rl)
					{
						n=p->first;
						break;
					}
				}

				// Ok, now we look for the first line break
				// up to here.

				while (++xpos < n)
				{
					if (breaks.at(xpos) !=
					    unicode_lb::none)
						break;
				}
				break;
			}

			// Found the first break before width.
			if (breaks.at(xpos) != unicode_lb::none)
				break;
			--xpos;
		}
	}

	return xpos;
}

size_t richtextfragmentObj::compute_split_pos_rl(dim_t desired_width) const
{
	auto &meta=string.get_meta();
	auto metab=meta.begin();
	auto metae=meta.end();

	if (desired_width > width)
		desired_width=width; // Shouldn't happen.

	// Find the index of the character that crosses desired_width
	size_t xpos=horiz_info.find_x_pos_right(width-desired_width);

	// Find this character's metadata.
	auto p=richtextstring::meta_upper_bound_by_pos(meta, xpos);

	if (p > metab)
		--p;

	// Did we land in the middle of left-to-right text? If so, we
	// won't break inside it.

	if (!p->second.rl)
	{
		while (1)
		{
			if (p == metae)
			{
				// Left to right text at the end. Find where
				// it begins, and break there. That's the
				// best we can do.

				while (1)
				{
					if (p == metab)
					{
						xpos=0;
						break;
					}

					// Start of right to left text, break
					// here.

					if (p[-1].second.rl)
					{
						if (p == metae)
							xpos=string.size();
						else
						{
							xpos=p->first;
						}
						break;
					}
					--p;
				}
				break;
			}

			// Right to left text begins (ends) here, so that'
			// the closest break point to "desired_width".

			if (p->second.rl)
			{
				xpos=p->first;
				break;
			}
			++p;
		}
	}
	else
	{
		// We landed in the middle of right-to-left text. Check to
		// see if there's any left-to-right text after. There /should/
		// be a break there, but this part of the code won't ass-ume it.

		size_t first_lr=string.size();
		while (p < metae)
		{
			if (!p->second.rl)
			{
				first_lr=p->first;
				break;
			}
			++p;
		}

		while (1)
		{
			if (xpos == first_lr)
			{
				// Reached either end of string or start
				// of left-to-right text. If not end of
				// string, we can break here.
				size_t n=string.size();

				if (xpos < n)
					break;

				first_lr=n;

				// Right to left text at the end of the line.
				// Find where the right-to-left text starts.

				for (p=metae; p != metab; )
				{
					--p;

					if (p->second.rl)
						first_lr=p->first;
					else break;
				}

				// Now look for the first breakable position
				// before first_lr.

				while (xpos > first_lr)
				{
					--xpos;
					if (breaks.at(xpos) !=
					    unicode_lb::none)
						break;
				}
				break;
			}

			if (breaks.at(xpos) != unicode_lb::none)
				break;
			++xpos;
		}
	}

	return xpos;
}

LIBCXXW_NAMESPACE_END
