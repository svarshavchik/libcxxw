/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextparagraph_H
#define richtextparagraph_H

#include "x/w/richtext/richtextparagraphfwd.H"
#include "richtext/richtextfragment.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "richtext/richtext_impl.H"
#include "x/w/connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

class paragraph_list;

//! A list of text fragments that ends with a mandatory line break.

class LIBCXX_HIDDEN richtextparagraphObj : virtual public obj {

public:

	//! This paragraph's place in the richtext object.

	//! richtext holds refs on its paragraphs. The entire rich text
	//! object is protected by a mutex lock. When the richtext gets
	//! destroyed, all of its paragraphs get destroyed. This native pointer,
	//! therefore, is always valid (so far).
	//!
	//! my_paragraph_number is this paragraph's number in
	//! my_richtext->paragraphs.

	richtext_implObj *my_richtext=nullptr;

	//! \see my_richtext
	size_t my_paragraph_number=0;

	//! Container for fragments
	typedef richtextfragmentObj::fragments_t fragments_t;

	//! Fragments (lines) in this paragraph.

	fragments_t fragments;

	//! Return the given fragment, with error checking

	inline auto get_fragment_iter(size_t n) const
	{
		return fragments.get_iter(n);
	}

	//! Return the given fragment, with error checking

	inline auto get_fragment_iter(size_t n)
	{
		return fragments.get_iter(n);
	}

	//! Return the given fragment, with error checking

	inline auto get_fragment(size_t n) const
	{
		return *get_fragment_iter(n);
	}

	//! Return the given fragment, with error checking

	inline auto get_fragment(size_t n)
	{
		return *get_fragment_iter(n);
	}

	//! The first fragment is fragment #n in rich text
	size_t first_fragment_n=0;

	//! How many fragments should be in this paragraph.

	//! When fragment_list goes out of scope, if this is different from
	//! fragments.size(), all subsequent paragraphs' first_fragment_n
	//! get updated. A paragraph must have at least one fragment, so the
	//! value of 0 is used to trigger recalculation.
	size_t n_fragments_in_paragraph=0;

	//! Also, we keep track of the first fragment's y position
	size_t first_fragment_y_position=0;

	//! We keep track of the total number of characters, here.

	size_t num_chars=0;

	//! And the first character in this paragraph is character #n
	size_t first_char_n=0;

	//! Constructor
	richtextparagraphObj();

	//! Destructor
	~richtextparagraphObj();

	//! Computed width of this text paragraph
	dim_t width=0;

	//! Maximum width of this paragraph, if it's just one line.
	dim_t maximum_width_if_one_line=0;

	//! Computed height of this text paragraph above the baseline of the last text fragment
	dim_t above_baseline=0;

	//! Computed height of this text paragraph below the baseline of the last text fragment
	dim_t below_baseline=0;

	//! The height of the tallest fragment in this paragraph
	dim_t height_of_tallest_fragment=0;

	//! The largest of all fragments' minimum_width.

	dim_t minimum_width=0;

	//! first_fragment_y_position plus height

	size_t next_paragraph_y_position() const
	{
		return first_fragment_y_position+
			dim_t::value_type(above_baseline) +
			dim_t::value_type(below_baseline);
	}

	//! Rewrap the paragraph to the given width, in pixels.

	//! Returns true if this paragraph was reformatted.

	bool rewrap(paragraph_list &my_paragraphs,
		    dim_t width,
		    richtext_insert_results &insert_results);

	//! Unwrap the paragraph completely

	//! This is equivalent to rewrap() with the maximum width
	bool unwrap(paragraph_list &my_paragraphs);

	//! Rewrap some entry in the fragments container

	//! Sets toosmall or toobig to indicated whether the following
	//! fragment was flowed into this one, at least partially; or
	//! whether this fragment was too big for the given width, and
	//! the excess got split off. The excess was split into a new fragment
	//! and placed into fragments just after this one.
	//!
	//! Note that if toosmall was set, it means that at least one following
	//! fragment was destroyed and rebuilt as a weak shadow of its
	//! former self.

	void rewrap_fragment(fragment_list &my_fragments,
			     dim_t width, size_t fragment_n,
			     bool &toosmall,
			     bool &toobig,
			     richtext_insert_results &insert_results);

 private:

	//! What rewrap_fragment_toobig returns.

	enum class toobig_results {
		//! We rewrapped it
		success,
		//! Could not rewrap it, too small or no breaking position
		failed,

		//! We rewrapped it, but we should try again, we may be able
		//! to rewrap it again.
		tryagain,
	};

	//! Rewrap a fragment that's too big.

	//! Called from rewrap_fragment. Returns tryagain if this fragment
	//! has bidirectional text in both directions. After it was wrapped
	//! it's possible that it now has bidirectional text in one direction
	//! that's still too big. We don't break in the middle of direction
	//! that's opposite to the paragraph embedding level, unless the entire
	//! fragment is in one direction. So, we could've broken things up
	//! now, and we now have a fragment in the same direction that can
	//! be broken up even further, so we should be called again.

	toobig_results rewrap_fragment_toobig(fragment_list &my_fragments,
					      const richtextfragment &f,
					      dim_t width,
					      bool &toosmall,
					      bool &toobig,
					      richtext_insert_results &results);

 public:
	/////////////////////////////////////////////////////////////////////

	//! Locate a fragment.

	//! Given an index of a character, searches through the fragments
	//! to find the highest first_char_n no greater than pos, and returns
	//! this fragment.
	//!
	//! Returns the fragment, and updates pos to the index of the given
	//! character in its paragraph.

	richtextfragment find_fragment_for_pos(size_t &pos) const;

	// Invoked only from fragment_list

	inline void adjust_char_count(ssize_t n)
	{
		num_chars += n;
		my_richtext->num_chars += n;
	}
};

#define richtextparagraph_H_defined
#include "richtext/richtextcursorlocationparagraphtext.H"

LIBCXXW_NAMESPACE_END

#endif
