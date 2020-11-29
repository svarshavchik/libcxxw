/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextcursorlocation.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtext_impl.H"
#include "assert_or_throw.H"
#include <functional>

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

void richtextcursorlocationObj::initialize(const richtextcursorlocation &clone)
{
	assert_or_throw(clone->my_fragment,
			"Internal: cloning an unattached location");
	initialize(clone->my_fragment, 0, new_location::lr);
	position=clone->position;
}

void richtextcursorlocationObj::initialize(richtextfragmentObj *fragment,
					   size_t offsetArg,
					   new_location location_option)
{
	initialize(fragment, offsetArg,
		   fragment->locations.insert(fragment->locations.end(),
					      ref<richtextcursorlocationObj>
					      (this)),
		   location_option);
}

void richtextcursorlocationObj::initialize(richtextfragmentObj *fragment,
					   size_t offsetArg,
					   richtextfragmentObj::locations_t
					   ::iterator new_iter,
					   new_location location_option)
{
	assert_or_throw(fragment &&
			fragment->my_paragraph &&
			fragment->my_paragraph->my_richtext,
			"Internal error: no rich text metadata in "
			"richtextcursorlocationObj::initialize");

	if (my_fragment)
		deinitialize(); // Repointing this one to another fragment.

	my_fragment=fragment;
	my_fragment_iter=new_iter;

	if (location_option == new_location::bidi &&
	    my_fragment->my_paragraph->my_richtext->rl())
	{
		assert_or_throw(offsetArg < my_fragment->string.size(),
				"Internal error: invalid initial location"
				" offset");
		offsetArg=my_fragment->string.size()-1-offsetArg;
	}

	position.offset=offsetArg;
	horiz_pos_no_longer_valid();
}

void richtextcursorlocationObj::deinitialize()
{
	if (my_fragment) // Initialized
		my_fragment->locations.erase(my_fragment_iter);
	my_fragment=nullptr;
}

dim_t richtextcursorlocationObj::get_horiz_pos(ONLY IN_THREAD)
{
	cache_horiz_pos(IN_THREAD);
	return dim_t::truncate(position.cached_horiz_pos);
}

void richtextcursorlocationObj::cache_horiz_pos(ONLY IN_THREAD)
{
	assert_or_throw(my_fragment, "Internal error in cache_horiz_pos(): my_fragment is not initialized");

	assert_or_throw(my_fragment->horiz_info.size() > position.offset,
			"Inernal error in cache_horiz_pos(): cursor location out of range");

	if (position.horiz_pos_is_valid)
		return;

	position.horiz_pos_is_valid=true;
	position.cached_horiz_pos=dim_squared_t::truncate
		(my_fragment->horiz_info.x_pos(position.offset)
		 + my_fragment->first_xpos(IN_THREAD));
	new_targeted_horiz_pos(IN_THREAD);
}

void richtextcursorlocationObj::new_targeted_horiz_pos(ONLY IN_THREAD)
{
	assert_or_throw(my_fragment, "Internal error in new_targeted_horiz_pos(): my_fragment is not initialized");

	position.set_targeted_horiz_pos(position.cached_horiz_pos
					+ my_fragment->horiz_info.width
					(get_offset())/2);
}

dim_squared_t richtextcursorlocationObj::get_targeted_horiz_pos(ONLY IN_THREAD)
{
	cache_horiz_pos(IN_THREAD);
	return position.targeted_horiz_pos;
}

void richtextcursorlocationObj
::set_targeted_horiz_pos(ONLY IN_THREAD,
			 dim_squared_t targeted_horiz_pos)
{
	assert_or_throw(my_fragment, "Internal error in set_targeted_horiz_pos(): my_fragment is not initialized");
	assert_or_throw(my_fragment->horiz_info.size() > 0,
			"Inernal error in set_targeted_horiz_pos(): empty fragment");

	position.horiz_pos_is_valid=true;

	auto h=dim_t::truncate(targeted_horiz_pos);

	auto first_xpos=dim_t::truncate(my_fragment->first_xpos(IN_THREAD));

	h=h<first_xpos ? 0:h-first_xpos;

	position.offset=
		my_fragment->horiz_info.find_x_pos(h);
	position.cached_horiz_pos=dim_squared_t::truncate
		(my_fragment->horiz_info.x_pos(position.offset)
		 + my_fragment->first_xpos(IN_THREAD));
	position.targeted_horiz_pos=targeted_horiz_pos;
}

// leftby1() and rightby1() are invoked from move().
//
// If horiz_pos_is_valid, horiz_pos gets updated too.

inline void richtextcursorlocationObj::leftby1(ONLY IN_THREAD)
{
	// Move by one character.

	position.cached_horiz_pos=
		dim_t::truncate
		(my_fragment->horiz_info.x_pos(--position.offset)
		 + my_fragment->first_xpos(IN_THREAD));
	new_targeted_horiz_pos(IN_THREAD);
}

inline void richtextcursorlocationObj::rightby1(ONLY IN_THREAD)
{
	position.cached_horiz_pos=
		dim_t::truncate
		(my_fragment->horiz_info.x_pos(++position.offset)
		 + my_fragment->first_xpos(IN_THREAD));
	new_targeted_horiz_pos(IN_THREAD);
}

void richtextcursorlocationObj::move(ONLY IN_THREAD, ssize_t howmuch)
{
	while (howmuch < 0)
	{
		assert_or_throw(my_fragment && my_fragment->my_paragraph &&
				my_fragment->my_paragraph->my_richtext &&
				position.offset
				< my_fragment->string.size(),
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

			leftby1(IN_THREAD);
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
			initialize(f, 0, new_location::lr);
			return; // Start of text
		}

		assert_or_throw(new_fragment && new_fragment->my_paragraph &&
				!new_fragment->my_paragraph->fragments.empty(),
				"Internal error in move(): null paragraph");

		auto new_fragment_text_size=
			new_fragment->string.size();
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

		initialize(new_fragment, new_fragment_text_size-1,
			   new_location::lr);
		++howmuch;
	}

	while (howmuch > 0)
	{
		assert_or_throw(my_fragment && my_fragment->my_paragraph &&
				my_fragment->my_paragraph->my_richtext &&
				position.offset < my_fragment->string.size(),
				"Internal error in move(): invalid offset");

		bool initialization_required=false;

		size_t chars_left;

		auto f=my_fragment;

		while ((size_t)howmuch >=
		       (chars_left=
			f->string.size()-position.offset))
		{
			initialization_required=true;

			// If we're at the start of the paragraph, and we're
			// moving forward by at least as much as the number of
			// characters in the paragraph, skip the entire
			// paragraph in one gulp.

			if (f->my_fragment_number == 0 &&
			    position.offset == 0 &&
			    (size_t)howmuch>=f->my_paragraph->num_chars)
			{
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
					continue;
				}
			}

			// We have enough left to move to the next line,
			// at least.

			auto next_f=f->next_fragment();

			if (!next_f)
			{
				// Last fragment, leave the cursor location
				// on the last character.

				assert_or_throw(!f->string.get_string().empty(),
						"Internal error in move(): "
						"empty fragment");
				auto pos=f->string.size()-1;

				initialize(f, pos, new_location::lr);
				return;
			}

			howmuch-=chars_left;
			f=next_f;
			position.offset=0;
		}

		if (initialization_required)
			initialize(f, 0, new_location::lr);

		// We can end up with howmuch being 0, if we landed it exactly.
		if (howmuch == 0)
			continue;

		// At this point we must be moving to the next character in
		// the same fragment. If howmuch was at least enough to go to
		// the next fragment we would've entered the previous if().

		// Advance to the next character in the fragment.

		rightby1(IN_THREAD);
		--howmuch;
	}
}

void richtextcursorlocationObj::up(ONLY IN_THREAD)
{
	auto targeted_horiz_pos=get_targeted_horiz_pos(IN_THREAD);

	auto new_fragment=my_fragment->prev_fragment();

	if (!new_fragment)
		return;
	initialize(new_fragment, 0, new_location::lr);
	set_targeted_horiz_pos(IN_THREAD, targeted_horiz_pos);
}

void richtextcursorlocationObj::down(ONLY IN_THREAD)
{
	auto targeted_horiz_pos=get_targeted_horiz_pos(IN_THREAD);

	auto new_fragment=my_fragment->next_fragment();

	if (!new_fragment)
		return;
	initialize(new_fragment, 0, new_location::lr);
	set_targeted_horiz_pos(IN_THREAD, targeted_horiz_pos);
}

void richtextcursorlocationObj::page_up(ONLY IN_THREAD, dim_t height)
{
	auto targeted_horiz_pos=get_targeted_horiz_pos(IN_THREAD);

	auto new_fragment=my_fragment->prev_fragment();

	if (!new_fragment)
		return;

	auto last_fragment=new_fragment;

	while (new_fragment)
	{
		last_fragment=new_fragment;

		if (new_fragment->height() >= height)
			break;

		height -= new_fragment->height();
		new_fragment=new_fragment->prev_fragment();
	}

	initialize(last_fragment, 0, new_location::lr);
	set_targeted_horiz_pos(IN_THREAD, targeted_horiz_pos);
}

void richtextcursorlocationObj::page_down(ONLY IN_THREAD, dim_t height)
{
	auto targeted_horiz_pos=get_targeted_horiz_pos(IN_THREAD);

	auto new_fragment=my_fragment->next_fragment();

	if (!new_fragment)
		return;

	auto last_fragment=new_fragment;

	while (new_fragment)
	{
		last_fragment=new_fragment;

		if (new_fragment->height() >= height)
			break;

		height -= new_fragment->height();
		new_fragment=new_fragment->next_fragment();
	}

	initialize(last_fragment, 0, new_location::lr);
	set_targeted_horiz_pos(IN_THREAD, targeted_horiz_pos);
}

bool richtextcursorlocationObj::moveto(ONLY IN_THREAD, coord_t x, coord_t y)
{
	// We expect that the requested x/y coordinates will be near this
	// existing location; so we just search.

	if (y < 0)
		y=0;

	assert_or_throw(my_fragment,
			"Internal error: my_fragment is null in moveto()");

	auto f=my_fragment;

	while ((size_t)(coord_t::value_type)y < f->y_position())
	{
		auto prevf=f->prev_fragment();

		if (!prevf) break; // Shouldn't really happen.

		f=prevf;
	}

	while (1)
	{
		auto nextf=f->next_fragment();

		if (!nextf)
			break;

		if ((size_t)(coord_t::value_type)y < nextf->y_position())
			break;

		f=nextf;
	}
	initialize(f, 0, new_location::lr);

	auto first_xpos=f->first_xpos(IN_THREAD);

	auto adjusted_x=dim_t::truncate(x-first_xpos);

	if (x < first_xpos)
		adjusted_x=0;

	set_targeted_horiz_pos(IN_THREAD, adjusted_x);

	return x >= first_xpos &&
		dim_t::truncate(adjusted_x) < f->x_width(IN_THREAD);
}

void richtextcursorlocationObj::start_of_line()
{
	assert_or_throw(my_fragment &&
			my_fragment->string.size() > 0,
			"Internal error in start_of_line(): invalid offset");

	position.offset=0;
	position.targeted_horiz_pos=0;
	horiz_pos_no_longer_valid();
}

void richtextcursorlocationObj::end_of_line()
{
	assert_or_throw(my_fragment &&
			my_fragment->string.size() > 0,
			"Internal error in end_of_line(): invalid offset");

	position.offset=my_fragment->string.size()-1;
	position.targeted_horiz_pos=dim_squared_t::truncate(~0);
	horiz_pos_no_longer_valid();
}

void richtextcursorlocationObj::inserted_at(ONLY IN_THREAD,
					    size_t pos,
					    size_t nchars,
					    dim_t extra_width)
{
	if (position.offset >= pos)
	{
		if (do_not_adjust_in_insert)
			return;

		position.offset += nchars;

		if (position.horiz_pos_is_valid)
		{
			position.cached_horiz_pos += extra_width;
			new_targeted_horiz_pos(IN_THREAD);
		}
		else
		{
			cache_horiz_pos(IN_THREAD);
		}
	}
}

bool richtextcursorlocationObj::same(const richtextcursorlocationObj &b) const
{
	assert_or_throw(my_fragment && b.my_fragment &&
			my_fragment->my_paragraph &&
			b.my_fragment->my_paragraph &&
			my_fragment->my_paragraph->my_richtext &&
			b.my_fragment->my_paragraph->my_richtext,
			"Uninitialized locations in same_position()");

	return std::equal_to<void>()(my_fragment, b.my_fragment) &&
		position.offset == b.position.offset;
}

std::ptrdiff_t richtextcursorlocationObj::compare(const richtextcursorlocationObj &b) const
{
	assert_or_throw(my_fragment && b.my_fragment,
			"Uninitialized locations in compare_location()");

	auto a_index=my_fragment->index(),
		b_index=b.my_fragment->index();

	if (a_index < b_index)
		return -(b_index-a_index+1);

	if (a_index > b_index)
		return a_index-b_index+1;

	return position.offset < b.position.offset ? -1:
		position.offset > b.position.offset ? 1:0;
}

void richtextcursorlocationObj::mirror_position(internal_richtext_impl_t::lock
						&)
{
	if (my_fragment->my_paragraph->my_richtext->rl())
		position.offset=my_fragment->string.size()-1-position.offset;
	horiz_pos_no_longer_valid();
}

void richtextcursorlocationObj::reposition(const richtextfragment &new_fragment,
					   size_t new_pos)
{
	assert_or_throw(new_fragment->string.size() > new_pos,
			"attempting to move to nonexistent position");
	my_fragment->move_location(my_fragment_iter, new_fragment);
	position.offset=new_pos;
	horiz_pos_no_longer_valid();
}

LIBCXXW_NAMESPACE_END
