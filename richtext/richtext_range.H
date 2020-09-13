/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_range_H
#define richtext_range_H

#include "x/w/input_field_lock.H"
#include "x/w/impl/richtext/richtextcursorlocationfwd.H"
#include "x/w/impl/richtext/richtextstringfwd.H"
#include "x/w/impl/richtext/richtextfragmentfwd.H"

#include <courier-unicode.h>
#include <functional>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN richtext_range;

//! Define the boundaries of bidirectional richtext

//! Helper logic used by get()
//!
//! Encapsulates the logic to take a starting and ending richtextcursorlocation
//! and then define, for each line fragment between them, which parts of the
//! line comprise the bidirectional text, and in which order.
//!
//! The constructor takes two richtextcursorlocations, and rearranges them
//! so that location_a is always the starting range and location_b is the
//! ending range.
//!
//! Depending on the paragraph embedding level and other factors, on the
//! line fragment where the range starts it's going to be either the characters
//! to the left or the right of the range start, that belong to the range;
//! similar to the line fragment where the range ends.
//!
//! And in between, even though each line is a part of the range the contents
//! may need to be combined in rearranged order.
//!
//! Subclass implements range(), which receives a single richtextstring, and
//! a starting position and the number of characters that comprise the
//! bidirectional text.
//!
//! After constructing the richtext_range, diff is set to the result of
//! compare(). If complete_line() returned true the range is entirely in a
//! single line fragment and it called add() to define it.
//!
//! Otherwise classify_fragment() determines whether the line fragment
//! consists of left-to-right or right-to-left text. Not all bidirectional
//! rules get encapsulated here. Using classify_fragment(), get() makes
//! a pass over the contents of the richtext, calling lr_lines() and rl_lines()
//! to define consecutive ranges of left-to-right and right-to-left text, in
//! logical order. Here we extract the appropriate part of each line,
//! usually the whole line. But if the line happens to be where the get
//! range starts and ends, we trim off the parts we will not extract, and
//! call range().
//!
//! Furthermore, in a right-to-left paragraph, range() skips the
//! \n at the beginning of the last line in the paragraph. get() handles
//! the logic for extracting it at the right time.

struct richtext_range {

	//! The richtext paragraph embedding direction.
	const unicode_bidi_level_t paragraph_embedding_level;

	//! Start and end of the range.
	richtextcursorlocation location_a, location_b;

	size_t location_a_offset, location_b_offset;

	//! Number of lines between location_a and location_b

	//! Returned by compare()
	std::ptrdiff_t diff;

	//! Constructor

	//! Receives a location of the starting and ending position to be
	//! extracted.

	richtext_range(const ref<richtext_implObj> &impl,
		       const richtextcursorlocation &a,
		       const richtextcursorlocation &b);

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
		const;

	//! Extract entire range from one complete line

	//! Extracts the contents between the starting and ending position
	//! if they're in the same fragment, returning true.
	//!
	//! Returns false if they're not, extracting nothing.

	bool complete_line();

	//! Add right to left lines to extracted text.

	//! "bottom" and "top" specifies the last and the first right-to
	//! left fragment. If this is a right-to-left oriented paragraph
	//! a non-null last_lr specifies that the right-to-left fragments
	//! are followed by one or more left-to-right fragments.
	//!
	//! last_lr is always null in left-to-right paragraphs.

	void rl_lines(const richtextfragmentObj *bottom,
		      const richtextfragmentObj *top,
		      const richtextfragmentObj *last_lr);

private:
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
	void rl_lines_in_lr(const richtextfragmentObj *bottom,
			    const richtextfragmentObj *top);
public:
	//! Add left-to-right oriented fragments to extracted text.

	//! "top" and "bottom" specifies the first and the last left-to
	//! right fragment. If this is a left-to-right oriented paragraph
	//! a non-null first_rl specifies that the left-to-right fragments
	//! were preceded by one or more right-to-left fragments, and this
	//! is first one of them.
	//!
	//! first_lr is always null in right-to-left paragraphs.

	void lr_lines(const richtextfragmentObj *first_rl,
		      const richtextfragmentObj *top,
		      const richtextfragmentObj *bottom);
private:
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

	void lr_lines_in_rl(const richtextfragmentObj *top,
			    const richtextfragmentObj *bottom);

	//! get()ing another line fragment of text.

	//! Appends another line to str.
	//!
	//! Checks if this line is contains the starting or the ending
	//! get() position, and if so trims off the parts before/after
	//! the range that we get().

	void line(const richtextfragmentObj *f,
		  unicode_bidi_level_t l);

	//! add() the next chunk of get() text.

	//! Append whatever in 'other', #n characters starting at position
	//! #start, to str.

	void add(const richtextstring &other,
		 size_t start,
		 size_t n) const;

	//! Define extracted character range.

	virtual void range(const richtextstring &other,
			   size_t start,
			   size_t n) const=0;
};

LIBCXXW_NAMESPACE_END

#endif
