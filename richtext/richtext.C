/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_impl.H"
#include "richtext/richtext_range.H"
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

halign richtextObj::get_alignment(ONLY IN_THREAD)
{
	impl_t::lock lock{impl};

	return (*lock)->alignment;
}

unicode_bidi_level_t richtextObj::get_paragraph_embedding_level(ONLY IN_THREAD)
{
	impl_t::lock lock{impl};

	return (*lock)->paragraph_embedding_level;
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
	return do_pos(l, get_location::bidi);
}

size_t richtextObj::do_pos(const richtextcursorlocation &l,
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

//! Retrieve text within the selected range

//! Uses richtext_range to logically collect the text into the richtextstring.
//!
//! Some of the high level lgoic
struct richtextObj::get_helper : richtext_range {

	//! The string getting extracted here.

	richtextstring &str;

	get_helper(richtextstring &str,
		   const ref<richtext_implObj> &impl,
		   const richtextcursorlocation &a,
		   const richtextcursorlocation &b)

		: richtext_range{impl->paragraph_embedding_level, a, b},
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
		// Edge case, for right-to-left embedding level we'll quietly
		// skip the \n at the beginning of the last line of the
		// paragraph. get() will take care of it.

		if (start == 0 && n > 0 &&
		    paragraph_embedding_level != UNICODE_BIDI_LR &&
		    other.get_string()[0] == '\n')
		{
			++start;
			--n;
		}

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

	auto diff=helper.diff;

	if (helper.complete_line())
		return str;

	// At this point, "diff" is the total number of line fragments,
	// at least 2, since the starting and ending position are on different
	// lines. If the ending position is on the line following the starting
	// position, then diff is 2, and we extract two lines (and let the
	// helper take care of the rest).

	if (helper.paragraph_embedding_level == UNICODE_BIDI_LR)
	{
		richtextfragmentObj *first_rl=nullptr;

		richtextfragmentObj *first_lr=nullptr, *last_lr=nullptr;

		auto f=helper.location_a->my_fragment;

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
				if (first_lr)
				{
					helper.lr_lines(first_rl,
							first_lr, last_lr);
					first_rl=first_lr=last_lr=nullptr;
				}
				if (!first_rl)
					// First right-to-left line.
					first_rl=f;
			}
			else
			{
				// Left to right line.

				if (!first_lr)
					first_lr=f;
				last_lr=f;
			}

			if (--diff == 0)
			{
				// End of paragraph. If we skipped some
				// lines, we'll do them here.

				if (first_lr)
					helper.lr_lines(first_rl,
							first_lr, last_lr);
				else if (first_rl)
					helper.rl_lines(f, first_rl, nullptr);
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

		auto f=helper.location_a->my_fragment;

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
			richtextfragmentObj *last_lr=nullptr;

			richtextfragmentObj *bottom_rl=nullptr;
			richtextfragmentObj *top_rl=nullptr;

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
					if (bottom_rl)
					{
						helper.rl_lines(bottom_rl,
								top_rl,
								last_lr);
						bottom_rl=top_rl=nullptr;
					}
					if (!last_lr)
						last_lr=f;
				}
				else
				{
					if (!bottom_rl)
						bottom_rl=f;
					top_rl=f;
				}

				if (n_lines_in_paragraph == 0)
				{
					// Paragraph began with left-to-right
					// text.
					if (bottom_rl)
						helper.rl_lines(bottom_rl,
								top_rl,
								last_lr);
					else if (last_lr)
						helper.lr_lines(nullptr,
								f, last_lr);
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
