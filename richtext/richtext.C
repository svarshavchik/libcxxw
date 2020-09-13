/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_impl.H"
#include "richtext/richtextparagraph.H"
#include "richtext/richtextfragment.H"
#include "richtext/richtextiterator.H"
#include "richtext/richtextcursorlocation.H"

LIBCXXW_NAMESPACE_START

richtextObj::richtextObj(richtextstring &&string,
			 const richtext_options &options)
	: richtextObj{ref<richtext_implObj>::create(std::move(string),
						    options),
	options.initial_width}
{
}

richtextObj::richtextObj(const ref<richtext_implObj> &impl,
			 dim_t initial_width)
	: impl{impl}
{
	impl_t::lock lock{this->impl};

	(*lock)->finish_initialization();
	(*lock)->rewrap(initial_width);
}

richtextObj::~richtextObj()=default;

// We must make sure that finish_initialization() gets invoked after the
// object gets constructed.

richtextObj::impl_t::lock::lock(impl_t &me)
	: internal_richtext_impl_t::lock(me)
{
}

size_t richtextObj::size(ONLY IN_THREAD)
{
	return read_only_lock([&]
			      (const auto &l)
			      {
				      return (*l)->num_chars;
			      });
}

void richtextObj::set(ONLY IN_THREAD, richtextstring &&string)
{
	impl_t::lock lock{impl};

	(*lock)->set(IN_THREAD, std::move(string));
}

bool richtextObj::rewrap(dim_t width)
{
	impl_t::lock lock{impl};

	return (*lock)->rewrap(width);
}

dim_t richtextObj::get_width()
{
	impl_t::lock lock{impl};

	return dim_t::truncate((*lock)->width());
}

std::pair<metrics::axis, metrics::axis>
richtextObj::get_metrics(dim_t preferred_width)
{
	impl_t::lock lock{impl};

	return (*lock)->get_metrics(preferred_width);
}

void richtextObj::text_width(const std::optional<dim_t> &s)
{
	read_only_lock([&]
		       (const auto &impl)
		       {
			       (*impl)->text_width=s;
		       });
}

void richtextObj::minimum_width_override(ONLY IN_THREAD, dim_t width)
{
	thread_lock(IN_THREAD,
		    [width]
		    (ONLY IN_THREAD, const auto &impl)
		    {
			    (*impl)->minimum_width_override=width;
		    });
}

richtextiterator richtextObj::begin()
{
	return at(0, new_location::bidi);
}

richtextiterator richtextObj::end()
{
	return at((size_t)-1, new_location::bidi);
}

richtextiterator richtextObj::at(size_t npos, new_location location_option)
{
	internal_richtext_impl_t::lock lock{impl};

	return at(lock, npos, location_option);
}

richtextiterator richtextObj::at(internal_richtext_impl_t::lock &lock,
				 size_t npos,
				 new_location location_option)
{
	size_t n_paragraph=(*lock)->find_paragraph_for_pos(npos);

	auto paragraph_iter=(*lock)->paragraphs.get_paragraph(n_paragraph);

	auto fragment=(*paragraph_iter)->find_fragment_for_pos(npos);

	auto s=fragment->string.size();

	if (s == 0)
		throw EXCEPTION("Internal error: empty rich text fragment in at().");

	if (npos >= s)
		npos=s-1;

	auto location=richtextcursorlocation::create();

	return richtextiterator::create(richtext(this),
					location,
					&*fragment,
					npos,
					location_option);
}

void richtextObj::insert_at_location(ONLY IN_THREAD,
				     impl_t::lock &lock,
				     const richtext_insert_base &new_text)
{
	(*lock)->insert_at_location(IN_THREAD,
				    new_text);
}

void richtextObj::remove_at_location(ONLY IN_THREAD,
				     impl_t::lock &lock,
				     const richtextcursorlocation &a,
				     const richtextcursorlocation &b)
{
	return (*lock)->remove_at_location(a, b);
}

void richtextObj::replace_at_location(ONLY IN_THREAD,
				      impl_t::lock &lock,
				      const richtext_insert_base &new_text,
				      const richtextcursorlocation &remove_from,
				      const richtextcursorlocation &remove_to)
{
	return (*lock)->replace_at_location(IN_THREAD,
					    new_text, remove_from, remove_to);
}

size_t richtextObj::pos(const internal_richtext_impl_t::lock &lock,
			const richtextcursorlocation &l)
{
	return do_pos(&*l, get_location::bidi);
}

size_t richtextObj::do_pos(const richtextcursorlocationObj *l,
			   get_location location_option)
{
	assert_or_throw
		(l->my_fragment &&
		 l->my_fragment->string.size() > l->get_offset() &&
		 l->my_fragment->my_paragraph &&
		 l->my_fragment->my_paragraph->my_richtext,
		 "Internal error in pos(): invalid cursor location");

	auto offset=l->get_offset();

	if (location_option == get_location::bidi &&
	    l->my_fragment->my_paragraph->my_richtext
	    ->rl())
		offset=l->my_fragment->string.size()-1-offset;

	return offset+l->my_fragment->first_char_n +
		l->my_fragment->my_paragraph->first_char_n;
}

//! Helper logic used by get()

//! get() makes the initial pass over the contents of richtext for extracting,
//! making a pass over each line/fragment in the range to be get-ted, in the
//! correct order. Here we extract the appropriate part of each line,
//! usually the whole line. But if the line happens to be where the get
//! range starts and ends, we trim off the parts we will not extract.

struct richtextObj::get_helper_base {

	//! The richtext paragraph embedding direction.
	const unicode_bidi_level_t paragraph_embedding_level;

	//! Start and end of the range.
	const richtextcursorlocationObj *location_a, *location_b;

	//! Number of lines between location_a and location_b

	//! Returned by compare()
	std::ptrdiff_t diff;

	//! Constructor

	//! Receives a location of the starting and ending position to be
	//! extracted.

	get_helper_base(const ref<richtext_implObj> &impl,
			const richtextcursorlocation &a,
			const richtextcursorlocation &b)
		: paragraph_embedding_level{impl->paragraph_embedding_level},
		  location_a{&*a},
		  location_b{&*b},
		  diff{location_a->compare(*location_b)}
	{
		if (diff == 0)
			return;

		// Make sure we go from a to b.

		if (diff > 0)
		{
			std::swap(location_a, location_b);
		}
		else
		{
			diff= -diff;
		}

		assert_or_throw(location_a->my_fragment &&
				location_b->my_fragment,
				"Internal error: uninitialized fragments"
				" in get()");
	}

	//! Pedantic pointer comparison

	//! We compare each extracted line/fragment pointer to the
	//! starting/ending get position. To be pedantic, we must use
	//! std::equal_to to correctly implement total order as it comes
	//! to pointer comparisons.

	std::equal_to<const richtextfragmentObj *> compare_fragments;

	//! Classify a fragment as being either left-to-right or right-to-left.

	//! The criteria differs, slightly, depending on the paragraph
	//! embedding level.

	unicode_bidi_level_t classify_fragment(const richtextfragmentObj *f)
		const
	{
		auto level=paragraph_embedding_level;

		if (level == UNICODE_BIDI_LR)
		{			// Compute the effective embedding level. if
			// this fragment's direction is BOTH, we will
			// consider it left-to-right text.
			//
			// Note that this means that after we see one or
			// more right-to-left lines, if the last right to left
			// line has the remainder of the right-to-left text,
			// followed by resumption of left-to-right text, it
			// will be considered left to right, here.

			return f->string.embedding_level(level);
		}

		// Figure out whether this line is left to right
		// or right to left.

		switch (f->string.get_dir()) {
		case richtext_dir::lr:
			level=UNICODE_BIDI_LR;
			break;
		case richtext_dir::rl:
			level=UNICODE_BIDI_RL;
			break;
		case richtext_dir::both:

			// We will consider this line a
			// left to right line only if it ENDS
			// with left to right text, which
			// might mean that it's wrapped from
			// the preceding chunk of left to right
			// text.
			auto &m=f->string.get_meta();
			auto b=m.begin();
			auto e=m.end();

			if (b != e && !e[-1].second.rl)
				level=UNICODE_BIDI_LR;
			break;
		}

		return level;
	}

	//! Extract right-to-left lines in left-to-right text.

	//! The paragraph embedding level is left to right, but we have one
	//! or more lines consisting of right-to-left text.
	//!
	//! Original text: "lllr rr rrr rrrr".
	//!
	//! Because it's right-to-left, when we wrap it across multiple
	//! lines this winds up as:
	//!
	//! lll
	//! rrrr
	//! rrr
	//! rr
	//! r
	//!
	//! We already extracted "lll", and we are now reconstituting the
	//! right to left next. So we need to extract right to left text
	//! from the bottom up.
	//!
	//! We have the "bottom" and "top" lines, so we start at the bottom
	//! and work our way to the top.
	void rl_line(const richtextfragmentObj *bottom,
		     const richtextfragmentObj *top)
	{
		while(1)
		{
			assert_or_throw(bottom != 0,
					"Internal error: null rl_line");
			line(bottom, UNICODE_BIDI_RL);

			if (compare_fragments(bottom, top))
				break;
			bottom=bottom->prev_fragment();
		}
	}

	//! Extract left-to-right lines in right-to-left text

	//! The paragraph embedding level is right to left, so if the original
	//! text was:
	//!
	//! rrr rr rllll lll ll l
	//!
	//! This was wrapped as
	//!
	//! llll
	//! lll
	//! ll
	//! l
	//! r
	//! rr
	//! rrr
	//!
	//! This is now being extracted, and get() walks its way up from the
	//! end of the paragraph to the beginning, but found a range of
	//! left-to-right oriented lines.
	//!
	//! Extract them, top to bottom.

	void lr_line(const richtextfragmentObj *top,
		     const richtextfragmentObj *bottom)
	{
		// The bottom line may not be entirely left-to-right. The
		// left to right portion would be at the end of the line.
		//
		// Start from the end of the line, and find where the left
		// to right text begins.

		auto &m=bottom->string.get_meta();
		auto b=m.begin(), e=m.end();

		while (b != e && !e[-1].second.rl)
			--e;

		// Right to left text begins to the right of "rl_begin".
		size_t rl_begin = e == m.end() ? bottom->string.size():e->first;

		// If bottom happens to be the end location of the get()
		// range, note the offset in the bottom fragment.
		//
		// Also, make a copy of location_b AND RESTORE IT before
		// we return, the logic below may modify it temporarily.

		size_t o=location_b->get_offset();

		auto copy=location_b;

		ptr<richtextcursorlocationObj> clone;

		// If "bottom" is the fragment with the end of the get()
		// range, and the end of the get() range is to the left
		// of rl_begin, it's grabbing some right-to-left text, and
		// since we are extracting the right-to-left text from the
		// bottom and going our way up, we need to grab the
		// portion between location_b and rl_begin, as the first
		// order of business.
		//
		// We'll then temporary move location_b, the ending get()
		// location, to rl_begin, so that the existing logic
		// below, when it eventually gets called for the bottom
		// line, ends up extracting just the left-to-right text.

		if (compare_fragments(bottom, location_b->my_fragment) &&
		    o < rl_begin)
		{
			add(bottom->string, o+1, rl_begin-(o+1));

			auto temp_end=richtextcursorlocation::create();

			// We need to have the temporary location_b begin
			// just to the left of rl_begin, since the logic
			// in line() trims off everything from the beginnin
			// of the line up to and including the ending
			// position.

			temp_end->initialize(location_b->my_fragment,
					     rl_begin-1,
					     new_location::lr);
			clone=temp_end;
			location_b= &*temp_end;
		}

		// With that out of the way, extract the lines of left-to-right
		// text, from top to bottom.

		while(1)
		{
			assert_or_throw(top != 0,
					"Internal error: null rl_line");

			// If this line is the bottom line of the get() range
			// we only need to extract everything on or after
			// the ending position.

			if (compare_fragments(top, location_b->my_fragment))
			{
				size_t end=location_b->get_offset()+1;
				add(top->string,
				    end,
				    top->string.size()-end);
			}
			else if (compare_fragments(top,
						   location_a->my_fragment))
			{
				// If this line is the top line of the get()
				// range, extract everything starting with
				// pos, to the end of the line.

				auto pos=location_a->get_offset();

				// Edge case, right to left paragraph embedding
				// level results in \n at the beginning of the
				// line. This is the end of the paragraph,
				// so if our starting extracting position is
				// here, we don't really extract anything
				// (get(), below, will take care of extracting
				// the \n).

				if (pos > 0 ||
				    top->string.get_string().at(0) != '\n')
				{
					add(top->string,
					    pos,
					    top->string.size()-pos);
				}
			}
			else
			{
				line(top, UNICODE_BIDI_LR);
			}
			if (compare_fragments(bottom, top))
				break;
			top=top->next_fragment();
		}

		location_b=copy;
	}

	//! get()ing another line fragment of text.

	//! Appends another line to str.
	//!
	//! Checks if this line is contains the starting or the ending
	//! get() position, and if so trims off the parts before/after
	//! the range that we get().

	void line(const richtextfragmentObj *f,
		  unicode_bidi_level_t l)
	{
		if (compare_fragments(f, location_a->my_fragment))
		{
			// Starting location.

			auto o=location_a->get_offset();

			if (l == UNICODE_BIDI_LR)
			{
				// For left-to right text we add() only from the
				// starting location to the end of the line.
				add(f->string, o, f->string.size()-o);
			}
			else
			{
				// For right to left text we add from the
				// beginning of the line up to *and including*
				// the starting location. So we add +1 to o.
				add(f->string, 0, o+1);
			}
			return;
		}

		if (compare_fragments(f, location_b->my_fragment))
		{
			// Ending location
			auto o=location_b->get_offset();

			if (l == UNICODE_BIDI_LR)
			{
				// Left to right text, add() only from the
				// beginning of the line to the ending location.
				add(f->string, 0, o);
			}
			else
			{
				// Right to left text, so to the right of 'o'
				// is what we get(), starting at o+1.
				add(f->string, o+1, f->string.size()-(o+1));
			}
			return;
		}
		add(f->string, 0, f->string.size());
	}

	//! add() the next chunk of get() text.

	//! Append whatever in 'other', #n characters starting at position
	//! #start, to str.

	void add(const richtextstring &other,
		 size_t start,
		 size_t n) const
	{
		size_t s=other.size();

		assert_or_throw(start <= s &&
				(s-start) >= n,
				"Internal error: invalid get string offset");

		// Edge case, for right-to-left embedding level we'll quietly
		// skip the \n at the beginning of the last line of the
		// paragraph.
		if (start == 0 && n > 0 &&
		    paragraph_embedding_level != UNICODE_BIDI_LR &&
		    other.get_string()[0] == '\n')
		{
			++start;
			--n;
		}

		range(other, start, n);
	}

	//! Define extracted character range.

	virtual void range(const richtextstring &other,
			   size_t start,
			   size_t n) const=0;
};

struct richtextObj::get_helper : get_helper_base {

	//! The string getting extracted here.

	richtextstring &str;

	get_helper(richtextstring &str,
		   const ref<richtext_implObj> &impl,
		   const richtextcursorlocation &a,
		   const richtextcursorlocation &b)

		: get_helper_base{impl, a, b},
		  str{str}
	{
		// Estimate how big ret will be. We can compute the
		// number of characters exactly. Use the number of
		// lines as the estimate for the number of metadata
		// changes. richtextstring's insert() is optimized
		// to avoid coalescing the metadata until it's
		// needed. So it will end up inserting a metadata
		// record for every line, initially.
		//
		// TODO: when we support rich text editing, we'll
		// need to add some additional overhead, here.

		auto pos1=richtextObj::do_pos(location_a, get_location::bidi);
		auto pos2=richtextObj::do_pos(location_b, get_location::bidi);

		auto index1=location_a->my_fragment->index();
		auto index2=location_b->my_fragment->index();

		if (pos1 > pos2)
			std::swap(pos1, pos2);
		if (index1 > index2)
			throw EXCEPTION("Internal error in get():"
					" inconsistent cursor"
					" locations");
		str.reserve(pos2-pos1+1, index2-index1+1);
	}

	//! Collect extracted characters into the richtextstring we're building.

	void range(const richtextstring &other,
		   size_t start,
		   size_t n) const override
	{
		if (n == 0)
			return;

		if (start == 0 && n == other.size())
		{
			str.insert(str.size(), other);
			return;
		}
		str.insert(str.size(), {other, start, n});
	}
};

richtextstring richtextObj::get(const internal_richtext_impl_t::lock &lock,
				const richtextcursorlocation &a,
				const richtextcursorlocation &b)
{
	richtextstring str;

	get_helper helper{str, (*lock), a, b};

	if (helper.diff == 0)
		return str; // Too easy

	auto &location_a=helper.location_a;
	auto &location_b=helper.location_b;
	auto &diff=helper.diff;

	// And now that the buffers are ready and waiting...

	if (diff == 1)
	{
		// On the same line.

		if ((*lock)->paragraph_embedding_level == UNICODE_BIDI_LR)
		{
			// Left to right text, get() characters starting
			// with the starting position, location_a, until
			// (but not including) location_b.
			helper.add(location_a->my_fragment->string,
				   location_a->get_offset(),
				   location_b->get_offset()-
				   location_a->get_offset());
		}
		else
		{
			// Right to left text. get() characters starting
			// (but not including) the starting position,
			// location_a, until and including location b.

			helper.add(location_a->my_fragment->string,
				   location_a->get_offset()+1,
				   location_b->get_offset()-
				   location_a->get_offset());
		}
		return str;
	}

	// At this point, "diff" is the total number of line fragments,
	// at least 2, since the starting and ending position are on different
	// lines. If the ending position is on the line following the starting
	// position, then diff is 2, and we extract two lines (and let the
	// helper take care of the rest).

	if (helper.paragraph_embedding_level == UNICODE_BIDI_LR)
	{
		richtextfragmentObj *first_rl=0;

		auto f=location_a->my_fragment;

		// We know extract left-to-right text, from the starting
		// fragment to the ending fragment.
		//
		// When we find a fragment of right to left text, we stop,
		// count the number of right-to-left lines, then extract
		// the right to left lines from the bottom up.
		//
		// If the original rich text was
		//
		// ll lllr rr rrr rrrr
		//
		// Then we wrapped it
		//
		// ll
		// lll
		// rrrr
		// rrr
		// rr
		// r
		//
		// So, after extracting the left to right text, when we find
		// right to left lines we skip ahead to the last one, "r",
		// then work our way up.

		do
		{
			assert_or_throw(f != NULL,
					"Internal error: NULL fragment");

			auto paragraph_embedding_level=
				helper.classify_fragment(f);

			if (paragraph_embedding_level != UNICODE_BIDI_LR)
			{
				if (!first_rl)
					// First right-to-left line.
					first_rl=f;
			}
			else
			{
				// If we just seen at least one right to
				// left line, process them.

				if (first_rl)
				{
					// Before processing all the pure
					// right-to-left lines, if the
					// first left-to-right line has some
					// leading right-to-left text, it's
					// part of the right-to-left sequence
					// and since right-to-left text is
					// processed from bottom up we just
					// handle it ourselves, here.
					//
					// But first, also check if the entire
					// get() range also ends on this
					// fragment.
					size_t cutoff=f->string.size();

					if (helper.compare_fragments
					    (f, location_b->my_fragment))
					{
						cutoff=location_b->get_offset();
					}

					// If the line starts with some
					// right to left text, find where
					// left to right text starts.

					auto m=f->string.get_meta();
					auto b=m.begin(),
						e=m.end();

					while (b != e && b->second.rl)
						++b;

					// Ok, so if right-to-left text
					// ends before the cutoff, we emit
					// it in its entirety here, before
					// emitting the rest of rl_lines.
					// Otherwise we emit only up to the
					// cutoff.
					helper.add(f->string, 0,
						   b->first < cutoff ?
						   b->first : cutoff);

					// Now, emit all right-to-left lines.
					helper.rl_line(f->prev_fragment(),
						       first_rl);
					first_rl=nullptr;

					// And then emit everything on this
					// line after the end of the right
					// to left text, and before the cutoff
					// (the end of the line, or the end
					// of the get() range).
					if (cutoff > b->first)
						helper.add(f->string, b->first,
							   cutoff-b->first);
				}
				else
				{
					// Ordinary left to right line. Boring.
					helper.line(f,
						    paragraph_embedding_level);
				}
			}

			if (--diff == 0)
			{
				// End of paragraph. If we skipped some
				// right-to-left lines, we'll do them here.
				if (first_rl)
					helper.rl_line(f, first_rl);
			}

			f=f->next_fragment();
		} while (diff);
	}
	else
	{
		// Right-to-left text:
		//
		// r rr rrr rrrr
		//
		// gets wrapped as
		//
		// rrrr
		// rrr
		// rr
		// r
		//
		// So we reassemble it from the bottom up. HOWEVER: paragraph
		// breajs. Paragraph breaks are left in place, so we
		// scan ahead until either we find the end of the get() range
		// or the paragraph break. If paragraph break, we emit the
		// paragraph, the newline, then proceed to the next paragraph.

		auto f=location_a->my_fragment;

		while (diff)
		{
			size_t n_lines_in_paragraph=0;

			// Scan ahead until we exhaust #diff lines, or see
			// a paragraph break. In right to left text, the
			// paragraph break is the \n at the beginning of the
			// fragment, and not the end.

			auto end_of_paragraph=f;

			while (diff)
			{
				++n_lines_in_paragraph;

				end_of_paragraph=f;

				f=f->next_fragment();
				--diff;

				if (end_of_paragraph->string.get_string().at(0)
				    == '\n')
					break;
			}

			// Start here, and work our way up to where we started
			// (the start of the get() range, or the start() of a
			// paragraph.

			f=end_of_paragraph;
			richtextfragmentObj *last_lr=0;

			while (n_lines_in_paragraph)
			{
				--n_lines_in_paragraph;

				assert_or_throw
					(f != NULL,
					 "Internal error: NULL fragment");

				auto paragraph_embedding_level=
					helper.classify_fragment(f);

				// When we find a left-to-right line, we need
				// emit left-to-right text from top to bottom,
				// so we mark the first line we find it, then
				// keep going.

				if (paragraph_embedding_level ==
				    UNICODE_BIDI_LR)
				{
					if (!last_lr)
						last_lr=f;
				}
				else
				{
					if (last_lr)
					{
						// If we skipped over some
						// left-to-right lines, emit
						// them.
						helper.lr_line
							(f->next_fragment(),
							 last_lr);
						last_lr=0;
					}
					helper.line(f,
						    paragraph_embedding_level);
				}

				if (n_lines_in_paragraph == 0)
				{
					// Paragraph began with left-to-right
					// text.
					if (last_lr)
						helper.lr_line(f, last_lr);
				}

				f=f->prev_fragment();
			}

			f=end_of_paragraph;

			// If there are more lines to do, we must've emitted
			// an entire paragraph, and its last fragment must
			// begin with the paragraph break, \n, so emit it.
			if (diff)
				str.insert(str.size(), {f->string, 0, 1});
			f=f->next_fragment();
		}
	}

	return str;
}

ref<richtext_implObj> richtextObj::debug_get_impl(ONLY IN_THREAD)
{
	impl_t::lock lock{impl};

	return *lock;
}

LIBCXXW_NAMESPACE_END
