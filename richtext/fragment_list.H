/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fragment_list_H
#define fragment_list_H

#include "richtext/fragment_listfwd.H"
#include "richtext/richtextfragment.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"

LIBCXXW_NAMESPACE_START

//! A helper class that operates on \ref richtextfragmentObj "fragments".

//! fragment_list and const_fragment_list are the only classes
//! that have full access to a paragraph's fragments_t.

class const_fragment_list {

public:
	typedef richtextfragmentObj::fragments_t fragments_t;

	//! The \ref paragraph_list "paragraph list" must be instantiated, first.

	paragraph_list &my_paragraphs;

	//! This is a fragment list from this paragraph.
	richtextparagraphObj &paragraph;

	//! Constructor
	const_fragment_list(paragraph_list &my_paragraphsArg,
			    richtextparagraphObj &paragraphArg);

	~const_fragment_list();

 protected:

	//! Internal function: insert a new fragment.

	//! Inserts the fragment into paragraphs.fragments, set its
	//! my_fragment_number, and increment all successive paragraphs'
	//! my_fragment_number.

	void insert_no_recalculate(fragments_t::iterator at,
				   const richtextfragment &fragment)
		const;
 public:
	//! Internal function: append a new fragment.

	//! This is invoked only from append(), and from richtext->set(),
	//! as an optimized bulk_insert of new text.
	//!
	//! Calls insert_no_recalculate(), followed by
	//! paragraph.adjust_char_count().

	void append_no_recalculate(const richtextfragment &new_fragment)
		const;
};

//! A helper class that operates on \ref richtextfragmentObj "fragments".

//! fragment_list and const_fragment_list are the only classes
//! that have full access to a paragraph's fragments_t.

class fragment_list : public const_fragment_list {

 private:

	//! Internal flag set by the methods when the fragments were modified.

	//! Checked in the destructor
	bool size_changed=false;
 public:

	//! Invoked by my paragraph's rewrap() and unwrap().

	void fragments_were_rewrapped()
	{
		size_changed=true;
	}

	//! Constructor
	fragment_list(paragraph_list &my_paragraphsArg,
		      richtextparagraphObj &paragraphArg);

	void theme_was_updated(ONLY IN_THREAD,
			       const const_defaulttheme &new_theme);

	//! Destructor

	//! Invokes recalculate_size() if either theme_was_updated or
	//! size_changed.
	//!
	//! No further action gets taken if theme_was_updated. Otherwise
	//! examine all paragraphs that follow this fragment list's paragraph,
	//! and recalculate each successive paragraph's first_fragment_n,
	//! n_fragments_in_paragraph, and first_fragment_y_position.
	//!
	//! If there was any change, sets paragraph_list's size_changed flag
	//! as well, so that the entire rich text's size can get recomputed.
	//!
	//! If the destructor finds that the fragment list is empty,
	//! the paragraph gets removed from the richtext object.

	~fragment_list();

	//! Recalculate sizes of all fragments whose sizes have changed.

	//! Invoked from destructor, and from rewrap_fragment().

	void recalculate_needed_fragment_sizes();

	//! Insert a new fragment at the indicated position.

	//! The actual number of characters in the paragraph does not change.
	//!
	//! Calls insert_no_recalculate(), followed by
	//! recalculate_fragment_size() (for the new fragment),
	//! and sets the size_changed flag.

	void insert_no_change_in_char_count(fragments_t::iterator at,
					    const richtextfragment &fragment);

	//! Erase the fragment at the given position.

	//! Decrements all successive fragments' my_fragment_number.
	//! Removes the indicated fragment from paragraph.fragments, and
	//! sets the size_changed flag.

	void erase(fragments_t::iterator at);

	//! Append a new fragment.

	//! Like insert(), but uses append_no_recalculate() instead.
	void append(const richtextfragment &new_fragment);

 public:

	//! Returns paragraph.fragments.size()

	//! Only this function has full access to paragraph.fragments.

	size_t size() const;

	//! Split a fragment, put all remaining fragments into a new paragraph.

	//! Invoked by richtextfragmentObj::split(), which passes one of my
	//! paragraphs as split_after. That fragment was split in the middle,
	//! and all that there's left is the portion of the fragment before
	//! the split, and \c new_fragment is everything that was after the
	//! split.
	//!
	//! split() creates a new paragraph after the one with the split
	//! fragment, and invokes its fragment list's split_from().
	//! The new paragraph is empty. This adds the \c new_fragment and
	//! any and all fragments that follow \c split_after into the new
	//! paragraph, adjusting all character counts accordingly...

	void split_from(const richtextfragment &new_fragment,
			fragment_list &split_after_fragment_list,
			richtextfragmentObj *split_after);

	//! Split a fragment, put all remaining fragments into a new paragraph

	//! This is called from insert() when it is not splitting a fragment
	//! in a middle, but rather moving all fragments after split_after
	//! into a new paragraph.

	void split_from(fragment_list &split_after_fragment_list,
			richtextfragmentObj *split_after);

	// Move next paragraph's fragments to this paragraph.

	void join_next();

	//! Get rid of some fragments in this paragraph.

	//! Remove #n_fragments fragments in this paragraph, starting with
	//! fragment #first_fragment. If there are fewer than #n_fragments,
	//! starting with #first_fragment, remove #first_fragment, and all
	//! subsequent fragments. Returns the number of fragments removed.
	//!
	//! Any cursor locations in the removed fragment are moved to
	//! locations_to.

	size_t remove(size_t first_fragment,
		      size_t n_fragments,
		      richtextfragmentObj *locations_to);

	friend class paragraph_list;

	//! One of my fragments has changed.

	//! richtextfragment reports that the given fragment_number has
	//! changed. The change involved a change in the number of characters.
	//! This is also called from set(), when initializing the entire
	//! rich text object.

	void fragment_text_changed(size_t fragment_number,
				   ssize_t text_count_changed);

	//! Create and append a new fragment
	richtextfragment append_new_fragment(richtextstring &&);

private:

	//! Recalculate the dimensions of the given fragment

	void recalculate_fragment_size(size_t fragment_number);

	//! Recalculate the width and the height of this text paragraph.

	//! The width is the maximum width of all fragments. The last fragment's
	//! below baseline is our below_baseline. Its above_baseline, and
	//! all above+below of all other fragments, is our above_baseline.
	//! Also calculates maximum_width_if_one_line and
	//! height_of_tallest_fragment.

	bool recalculate_size();

	//! The meat of the recalculation.
	void recalculate_size(bool &width_changed,
			      bool &height_changed);
};

LIBCXXW_NAMESPACE_END

#endif
