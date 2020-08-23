/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_linebreak_info.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

// richtext_linebreak_info() constructs an instance of this class in
// automatic scope, which performs the actual calculations.

class calculate_linebreaks {

	//! Parameter to richtext_linebreak_info()
	const size_t skip;

	//! Parameter to richtext_linebreak_info()
	const size_t todo;

	//! Parameter to richtext_linebreak_info()
	unicode_lb * const ptr;

	//! Parameter to richtext_linebreak_info()
	size_t current_pos;

public:
	//! Everything happens in the constructor
	calculate_linebreaks(size_t skip,
			     size_t todo,
			     unicode_lb *ptr,
			     richtextstring * const *strings,
			     size_t n_strings);

	//! Save a value.

	//! Used to produce the output.
	inline void save(size_t pos, unicode_lb value) const
	{
		if (pos < skip)
			return;

		pos -= skip;

		if (pos >= todo)
			return;

		ptr[pos]=value;
	}
};

// Pseudo iterator for iterating over one sequence of text+metadata over
// a sequence of richtextstrings.
//
// The sequence of richtextstrings is given as an array of pointers.
//
// calculate_linebreaks receives this array to compute linebreaks for.
//
// They are virtually concatenated here, and we iterate over each metadata
// sequence, in turn.

struct richtext_meta_iter {

	// This is initialized to the array of pointers, and the number of
	// pointers.
	//
	// Iterating to the next pointer is done by incrementing
	// current_richtextstring, and decrementing n_stringsleft.
	//
	// The opening bid for the "ending iterator value" gets defined as
	// n_stringsleft of 0.

	richtextstring * const *current_richtextstring;
	size_t n_stringsleft;

	// If we are not at the ending iterating value, this is the
	// u32string part of the current_richtextstring.
	const std::u32string *current_string;

	// Once we start looking at current_string, cur_meta_p and cur_meta_end
	// is the richtextstring's metadata sequence.
	//
	// richtext_meta_iter iterates to the next value by incrementing
	// cur_meta_p, and if it is now cur_meta_end, we advance to the
	// next richtextstring.
	richtextstring::meta_t::const_iterator cur_meta_p, cur_meta_end;

	// The starting position of the first character in the current sequence.
	//
	// This starts at 0, and as we advance to the next metadata this gets
	// incremented by the number of characters we skipped over.
	size_t starting_pos=0;

	// Is this the ending iterator value()?

	bool is_end()
	{
		return n_stringsleft == 0
			|| (current_string=
			    &(*current_richtextstring)->get_string())->empty();
	}

	// Convenient shortcut, whether the current metadata is right to left.

	inline bool rl()
	{
		return cur_meta_p->second.rl;
	}

	// Advance to the next metadata.

	void next()
	{
		auto p=cur_meta_p;

		++p;

		// If we did not reach the last metadata in this string,
		// set cur_meta_p to p.

		if (p != cur_meta_end)
		{
			starting_pos += p->first - cur_meta_p->first;
			cur_meta_p=p;
			return;
		}

		// Otherwise we advance to the next string.
		starting_pos += current_string->size()-cur_meta_p->first;

		++current_richtextstring;
		--n_stringsleft;
		init();
	}

	// Initialize, after advancing to the next string.
	//
	// Also called to set things up after constructing the beginning
	// iterator value.

	void init()
	{
		if (is_end())
			return; // We're done.

		// is_end() also loads current_richtextstring. How convenient.

		auto &meta=(*current_richtextstring)->get_meta();

		cur_meta_p=meta.begin();
		cur_meta_end=meta.end();

		if (cur_meta_p == cur_meta_end)
			throw EXCEPTION("Internal error: unexpected empty "
					"metadata while calculating linebreaks"
					);
	}

	// Convenient shortcut.
	//
	// Get the starting and ending iterators for the character sequence
	// that corresponds to the current metadata.

	auto get_string_iter()
	{
		auto sb=current_string->begin();

		auto next_p=cur_meta_p;

		++next_p;

		return std::tuple{sb + cur_meta_p->first,
				sb + (next_p == cur_meta_end ?
				      current_string->size()
				      : next_p->first)};
	}
};

#if 0
{
#endif
}
namespace {
#if 0
}
#endif

// Generate linebreak values for left-to-right text. Straightforward.

struct left_to_right_line_breaks : unicode::linebreak_callback_base {

	calculate_linebreaks &info;

	size_t starting_pos;

	left_to_right_line_breaks(calculate_linebreaks &info,
				  size_t starting_pos)
		: info{info}, starting_pos{starting_pos}
	{
	}

	int callback(int intvalue) override
	{
		switch (intvalue) {
		case UNICODE_LB_NONE:
		case UNICODE_LB_ALLOWED:
		case UNICODE_LB_MANDATORY:
			break;
		default:
			throw EXCEPTION("Internal error: unknown linebreaking"
					" value");
		}

		auto value=static_cast<unicode_lb>(intvalue);

		info.save(starting_pos++, value);
		return 0;
	}
};

// Generate linebreak values for right-to-left text. A little bit tricker
//
// We are generating linebreak values for a subsection of the larger
// richtextstring starting at start_pos, and nchars in size.
//
// The linebreaking algorithms gets fed with the unicode characters in
// logical order, from the right end to the left end.
//
// We define the linebreak value of a particular character as indicating
// the break before it.
//
// The first character, for both left to right and right to left characters
// is always unicode_lb::allowed (higher level richtext code overrides the
// first character in the paragraph as either unicode_lb::always or
// unicode_lb::none, but here this is how we're looking at this).
//
// Therefore, the linebreaking values are set as follows:
//
// first character, then starting from the last character in the right-to-left
// sequence, going back towards the beginning. So that we end up with:
//
// - the 2nd character's linebreaking value is set for the last character
// in the sequence (before it, and after the 2nd char.
//
// - the 3rd character's linebreaking value is set for the 2nd last character,
// and so on.

struct right_to_left_line_breaks : unicode::linebreak_callback_base {

	calculate_linebreaks &info;

	size_t starting_pos;
	size_t n;
	size_t nchars;
	bool first;

	right_to_left_line_breaks(calculate_linebreaks &info,
				  size_t starting_pos,
				  size_t nchars)
		: info{info}, starting_pos{starting_pos}, n{0},
		  nchars{nchars}
	{
	}

	int callback(int intvalue) override
	{
		switch (intvalue) {
		case UNICODE_LB_NONE:
		case UNICODE_LB_ALLOWED:
		case UNICODE_LB_MANDATORY:
			break;
		default:
			throw EXCEPTION("Internal error: unknown linebreaking"
					" value");
		}

		auto value=static_cast<unicode_lb>(intvalue);

		info.save(starting_pos+n, value);
		n=(n + (nchars-1)) % nchars;
		return 0;
	}
};

calculate_linebreaks::calculate_linebreaks(size_t skip,
					   size_t todo,
					   unicode_lb *ptr,
					   richtextstring * const *strings,
					   size_t n_strings)
	: skip{skip},
	  todo{todo},
	  ptr{ptr},
	  current_pos{0}
{

	// Starting iterator, or the current iterator. initialize it.

	richtext_meta_iter current_iter{strings, n_strings};

	current_iter.init();

	std::vector<richtext_meta_iter> iters;

	while (!current_iter.is_end())
	{
		// We now look ahead, as long as the metadata is rendered
		// in the same direction.

		auto next_iter=current_iter;
		size_t n=0;

		while (next_iter.rl() == current_iter.rl())
		{
			++n;
			next_iter.next();
			if (next_iter.is_end())
				break;
		}

		// We now knoe how big the iters vector should be.

		iters.clear();
		iters.reserve(n);

		// Populate it.
		for (size_t i=0; i<n; ++i)
		{
			iters.emplace_back(current_iter);
			current_iter.next();
		}

		// Figure out which way this is rendered

		if (!iters[0].rl())
		{
			// Straightforward.

			left_to_right_line_breaks breaks{*this,
							 iters[0].starting_pos};

			for (size_t i=0; i<n; ++i)
			{
				auto [start_s, start_e]=
					iters[i].get_string_iter();

				breaks(start_s, start_e);
			}
			breaks.finish();
		}
		else
		{
			right_to_left_line_breaks breaks
				{*this, iters[0].starting_pos,
				 static_cast<size_t>(current_iter.starting_pos-
						     iters[0].starting_pos)};

			// Feed the characters in logical order.
			for (size_t i=0; i<n; ++i)
			{
				auto [start_s, start_e]=
					iters[n-1-i].get_string_iter();

				while (start_s != start_e)
				{
					breaks << *--start_e;
				}
			}
			breaks.finish();
		}

		// Always allow a break before the start of an individual
		// left to right or right to left sequence.
		save(iters[0].starting_pos, unicode_lb::allowed);
	}
}

#if 0
{
#endif
}

void richtext_linebreak_info(size_t skip,
			     size_t todo,
			     unicode_lb *ptr,
			     richtextstring * const *strings,
			     size_t n_strings)
{
	calculate_linebreaks calc{skip, todo, ptr, strings, n_strings};
}

LIBCXXW_NAMESPACE_END
