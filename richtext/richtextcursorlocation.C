/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtext_impl.H"
#include "assert_or_throw.H"

LIBCXXW_NAMESPACE_START

/////////////////////////////////////////////////////////////////////////////
//
// The actual location of the cursor

richtextcursorlocationObj::richtextcursorlocationObj()
{
}

richtextcursorlocationObj::~richtextcursorlocationObj() noexcept
{
}

void richtextcursorlocationObj::initialize(richtextfragmentObj *fragment,
					   size_t offsetArg)
{
	initialize(fragment, offsetArg,
		   fragment->locations.insert(fragment->locations.end(),
					      ref<richtextcursorlocationObj>
					      (this)));
}

void richtextcursorlocationObj::initialize(richtextfragmentObj *fragment,
					   size_t offsetArg,
					   richtextfragmentObj::locations_t
					   ::iterator new_iter)
{

	if (my_fragment)
		deinitialize(); // Repointing this one to another fragment.

	my_fragment=fragment;
	my_fragment_iter=new_iter;

	position.offset=offsetArg;
	update_horiz_pos();
}

void richtextcursorlocationObj::deinitialize()
{
	if (my_fragment) // Initialized
		my_fragment->locations.erase(my_fragment_iter);
	my_fragment=nullptr;
}

richtextcursorlocation richtextcursorlocationObj::clone() const
{
	auto l=richtextcursorlocation::create();

	l->position=position;
	return l;
}

void richtextcursorlocationObj::update_horiz_pos()
{
	assert_or_throw(my_fragment, "Internal error in update_horiz_pos(): my_fragment is not initialized");

	assert_or_throw(my_fragment->horiz_info.size() > position.offset,
			"Inernal error in update_horiz_pos(): cursor location out of range");

	position.horiz_pos=0;

	for (size_t i=0; i<position.offset; ++i)
	{
		position.horiz_pos += my_fragment->horiz_info.width(i);
		position.horiz_pos += my_fragment->horiz_info.kerning(i+1);
	}

	if (position.offset)
	{
		position.horiz_pos -= my_fragment->horiz_info.kerning(0);
		// Does not count
		position.horiz_pos += my_fragment->horiz_info.kerning(position.offset);
		// Does count
	}
	position.reset_horiz_pos();
}

void richtextcursorlocationObj::update_offset(dim_squared_t targeted_horiz_pos)
{
	assert_or_throw(my_fragment, "Internal error in update_offset(): my_fragment is not initialized");

	position.horiz_pos=0;
	position.offset=0;

	auto n=my_fragment->horiz_info.size();

	// Iterate until we cross targeted_horiz_pos;
	for ( ; position.offset+1 < n; )
	{
		auto next_horiz_pos=position.horiz_pos +
			my_fragment->horiz_info.width(position.offset) +
			my_fragment->horiz_info.kerning(position.offset+1);

		if (next_horiz_pos > targeted_horiz_pos)
		{
			// Choose the closest X position.
			if ( position.horiz_pos +
			     (next_horiz_pos-position.horiz_pos)/2 >=
			     targeted_horiz_pos)
				break;

			++position.offset;
			position.horiz_pos=next_horiz_pos;
			break;
		}
		++position.offset;
		position.horiz_pos=next_horiz_pos;
	}
	position.targeted_horiz_pos=targeted_horiz_pos;
}

void richtextcursorlocationObj::move(ssize_t howmuch)
{
	while (howmuch < 0)
	{
		assert_or_throw(my_fragment && my_fragment->my_paragraph &&
				my_fragment->my_paragraph->my_richtext &&
				position.offset
				< my_fragment->string.get_string().size(),
				"Internal error in move(): invalid offset");

		if (position.offset)
		{
			// We're not at a beginning of some fragment.

			if (howmuch + position.offset <= 0)
			{
				// But howmuch is big enough to move to at
				// least the beginning of the line.

				howmuch += position.offset;
				start_of_line();
				continue;
			}

			// Move by one character.
			// Subtract the current character's kerning.
			// Subtract the previous character's width.

			position.horiz_pos-=my_fragment->horiz_info.kerning(position.offset);
			--position.offset;
			position.horiz_pos-=my_fragment->horiz_info.width(position.offset);
			position.reset_horiz_pos();
			++howmuch;
			continue;
		}

		// At the beginning of a fragment. Advance by entire fragments,
		// as much as possible. From this point on, initialize() is
		// required before leaving this scope.

		auto f=my_fragment;

	start_of_line_going_backwards:

		auto new_fragment=f->prev_fragment();

		if (!new_fragment)
		{
			initialize(f, 0);
			return; // Start of text
		}

		assert_or_throw(new_fragment && new_fragment->my_paragraph &&
				!new_fragment->my_paragraph->fragments.empty(),
				"Internal error in move(): null paragraph");

		auto new_fragment_text_size=
			new_fragment->string.get_string().size();
		assert_or_throw(new_fragment_text_size,
				"Internal error in move(): empty fragment");

		// When leaving a paragraph, see if an entire paragraph can
		// be skipped.

		if (f->my_fragment_number == 0
		    && new_fragment->my_paragraph->num_chars<(size_t)-howmuch)
		{
			howmuch+=new_fragment->my_paragraph->num_chars;
			f=&**new_fragment->my_paragraph->fragments.get_iter(0);
			goto start_of_line_going_backwards;
		}

		if (new_fragment_text_size<(size_t)-howmuch)
		{
			// Can skip a fragment.
			howmuch+=new_fragment_text_size;
			f=new_fragment;
			goto start_of_line_going_backwards;
		}

		// Can't figure out what to do, move to the end of the
		// previous fragment, and try again.

		initialize(new_fragment, new_fragment_text_size-1);
		++howmuch;
	}

	while (howmuch > 0)
	{
		assert_or_throw(my_fragment && my_fragment->my_paragraph &&
				my_fragment->my_paragraph->my_richtext &&
				position.offset < my_fragment->string.get_string().size(),
				"Internal error in move(): invalid offset");

		auto chars_left=
			my_fragment->string.get_string().size()-position.offset;

		if (chars_left <= (size_t)howmuch)
		{
			// We have enough left to move to the next line,
			// at least.
			//
			// From this point on, initialize() must occur before
			// leaving this scope.

			auto f=my_fragment;

		start_of_line_going_forward:

			if (f->my_fragment_number == 0 &&
			    position.offset == 0 &&
			    (size_t)howmuch>=f->my_paragraph->num_chars)
			{
				// We have enough to skip an entire
				// paragraph.

				auto next_paragraph_number=
					f->my_paragraph->my_paragraph_number+1;

				if (next_paragraph_number <
				    f->my_paragraph->my_richtext->
				    paragraphs.size())
				{
					auto next_paragraph_iter=
						f->my_paragraph->my_richtext
						->paragraphs.get_paragraph
						(next_paragraph_number);

					assert_or_throw((*next_paragraph_iter)
							->fragments.size(),
							"Internal error in "
							"move(): empty "
							"paragraph");
					howmuch-=f->my_paragraph->num_chars;
					f=&**(*next_paragraph_iter)
						->fragments.get_iter(0);
					goto start_of_line_going_forward;
				}
			}

			chars_left= // This could've changed in the loop above.
				my_fragment->string.get_string().size()
				-position.offset;

			auto next_f=f->next_fragment();

			if (!next_f)
			{
				// Last fragment, leave the cursor location
				// on the last character.

				assert_or_throw(!f->string.get_string().empty(),
						"Internal error in move(): "
						"empty fragment");
				auto pos=f->string.get_string().size()-1;

				initialize(f, pos);
				return;
			}

			howmuch-=chars_left;
			f=next_f;
			position.offset=0;
			chars_left=f->string.get_string().size();

			// Is there enough left for at least another line?
			if (chars_left <= (size_t)howmuch)
				goto start_of_line_going_forward;

			// We entered this if() statement when howmuch was
			// enough to at least move the location to the next
			// line. Therefore, we must leave the scope in the
			// same state.

			initialize(f, 0);
		}

		// We can end up with howmuch being 0, if we landed it exactly.
		if (howmuch == 0)
			continue;

		// At this point we must be moving to the next character in
		// the same fragment. If howmuch was at least enough to go to
		// the next fragment we would've entered the previous if().

		// Advance to the next character in the fragment.

		// Add in this character's width.

		position.horiz_pos+=
			my_fragment->horiz_info.width(position.offset);
		++position.offset;

		// Add in the next character's kerning.

		position.horiz_pos+=
			my_fragment->horiz_info.kerning(position.offset);
		position.reset_horiz_pos();
		--howmuch;
	}
}

void richtextcursorlocationObj::start_of_line()
{
	assert_or_throw(my_fragment &&
			position.offset < my_fragment->string.get_string().size(),
			"Internal error in start_of_line(): invalid offset");

	position.offset=0;
	update_horiz_pos();
}

void richtextcursorlocationObj::end_of_line()
{
	assert_or_throw(my_fragment &&
			position.offset < my_fragment->string.get_string().size(),
			"Internal error in start_of_line(): invalid offset");

	position.offset=my_fragment->string.get_string().size()-1;
	update_horiz_pos();
	position.targeted_horiz_pos=~0;
}

LIBCXXW_NAMESPACE_END
