/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_insert.H"
#include "richtext/richtext_linebreak_info.H"

LIBCXXW_NAMESPACE_START

// Construct a richtextstring for the new paragraph fragment.
//
// Moves \n to the beginning of the string in right-to-left paragraphs

static inline auto create_new_fragment_text(richtextstring &string,
					    size_t i,
					    size_t j,
					    unicode_bidi_level_t
					    paragraph_embedding_level)
{
	// Normally the \n ends the paragraph. If the paragraph
	// embedding level is right-to-left, \n belongs at the
	// beginning of the paragraph.

	bool swap_newline=false;

	if (paragraph_embedding_level != UNICODE_BIDI_LR)
	{
		if (string.get_string().at(j-1) == '\n')
			swap_newline=true;
	}

	if (!swap_newline)
		return richtextstring{string, i, j-i};

	richtextstring rl_str{string, j-1, 1}; // The newline

	if (j-i > 1)
	{
		rl_str += richtextstring{string, i, j-i-1};
	}

	return rl_str;
}

create_fragments_from_inserted_text
::create_fragments_from_inserted_text(richtextstring &string,
				      const std::optional<unicode_bidi_level_t>
				      &new_paragraph_embedding_level,
				      unicode_bidi_level_t
				      &final_paragraph_embedding_level)
	: canonicalizer{string, new_paragraph_embedding_level}
{
#ifdef CFFIT_CONSTRUCTOR_DEBUG
	CFFIT_CONSTRUCTOR_DEBUG;
#endif

	final_paragraph_embedding_level=
		canonicalizer.paragraph_embedding_level();
}

create_fragments_from_inserted_text::~create_fragments_from_inserted_text()
=default;

bool create_fragments_from_inserted_text::has_paragraph_break() const
{
	if (canonicalizer.end())
		return false;

	auto &str=(*canonicalizer).get_string();

	return str.at(0) == '\n' ||
		str.at(str.size()-1) == '\n';

}

richtext_dir create_fragments_from_inserted_text::next_string_dir() const
{
	if (canonicalizer.end())
	{
		return canonicalizer.paragraph_embedding_level()
			== UNICODE_BIDI_LR
			? richtext_dir::lr : richtext_dir::rl;
	}

	return (*canonicalizer).get_dir();
}

richtextstring create_fragments_from_inserted_text::next_string()
{
	if (canonicalizer.end())
	{
		return {};
	}

	auto &current_string=*canonicalizer;

#ifdef CFFIT_NEXT_DEBUG
	CFFIT_NEXT_DEBUG;
#endif
	++canonicalizer;

	// See canonicalizer's docs.
	return std::move(current_string);
}


void richtext_insert_results::split(const richtextfragment &split_from,
				    size_t pos,
				    size_t count,
				    const richtextfragment &split_to)
{
	// The inserted range that was split off
	range split{pos, pos+count};

	auto from_range_iter=inserted_range.find(split_from);

	if (from_range_iter == inserted_range.end())
		return; // Nothing to split, for sure.

	auto &from_range=from_range_iter->second;

	// Extract the split range, shift it to 0, and install it as the
	// split_to's range.
	auto split_range=from_range.extract(split);
	split_range.shift(-pos);

	if (!split_range.empty())
		inserted_range.emplace(split_to, std::move(split_range));

	// Now we need to update the from_range, this part is gone:
	from_range.remove(split);

	// But now everything after pos+count was shifted over.

	auto [from_begin, from_end]=from_range.range();

	range tail{pos+count, from_end};

	if (tail.begin < tail.end) // Something there.
	{
		auto tail_range=from_range.extract(tail);

		from_range.remove(tail);

		tail_range.shift(-count);
		from_range << tail_range;
	}
	if (from_range.empty())
		inserted_range.erase(from_range_iter);
}

void richtext_insert_results::merged(const richtextfragment &merged_from,
				     const richtextfragment &merged_to,
				     size_t merged_to_pos)
{
	auto merged_from_iter=inserted_range.find(merged_from);
	auto merged_to_iter=inserted_range.find(merged_to);

	auto end=inserted_range.end();

	if (merged_from_iter == end && merged_to_iter == end)
		return;

	if (merged_to_iter != end)
	{
		// Need to shift the merged_to ranges after meged_to_pos;
		range shift_range{merged_to_pos,
			merged_to->string.size()};

		auto &merged_to_range=merged_to_iter->second;

		// In the merged_to, we need to keep any range before the
		// merged_to_pos and then anything on or after merged_to_pos
		// gets shifted by what's in merged_from.

		assert_or_throw(shift_range.begin <= shift_range.end,
				"internal error: inconsistent merged() range");

		auto shifted_range=merged_to_range.extract(shift_range);
		merged_to_range.remove(shift_range);

		// Shift it over and put it back in.
		shifted_range.shift(merged_from->string.size());
		merged_to_range << shifted_range;
	}
	else
	{
		// We must be inserting something.

		merged_to_iter=inserted_range.emplace(merged_to,
						      sorted_range<range>{})
			.first;
	}

	// Now grab the merged_from range. This is a removed fragment,
	// so we can shift its range in place, before getting rid of it.

	if (merged_from_iter != end)
	{
		auto &merged_from_range=merged_from_iter->second;

		merged_from_range.shift(merged_to_pos);
		merged_to_iter->second << merged_from_range;
		inserted_range.erase(merged_from_iter);
	}
}

void richtext_insert_results::removed(const richtextfragment &f)
{
	auto p=inserted_range.find(f);

	if (p != inserted_range.end())
		inserted_range.erase(p);
}

void richtext_insert_results::removed(const richtextfragment &f,
				      size_t pos, size_t nchars)
{
	auto p=inserted_range.find(f);

	if (p == inserted_range.end())
		return; // Not tracking done, easy.

	auto &f_range=p->second;

	f_range.remove(range{pos, pos+nchars});

	if (f_range.empty())
	{
		// Nothing left here
		inserted_range.erase(p);
		return;
	}

	auto [total_begin, total_end]=f_range.range();

	if (total_end <= pos+nchars)
		// Remaining stuff in the end is before the removal point.
		return;

	// Take the range after the removal point, and move it by -nchars.
	// First, extract the range.
	range tail_range{pos+nchars, total_end};

	auto tail=f_range.extract(tail_range);

	// Remove it from the fragment being tracked.
	f_range.remove(tail_range);

	// Shift it
	tail.shift(-(ssize_t)nchars);

	// Put it back in.
	f_range << tail;
}

LIBCXXW_NAMESPACE_END
