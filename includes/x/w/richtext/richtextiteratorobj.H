/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_richtextiteratorobj_H
#define x_w_richtextiteratorobj_H

#include <x/w/richtext/richtextiteratorfwd.H>
#include <x/w/richtext/richtextfwd.H>
#include <x/w/richtext/richtext_insertfwd.H>
#include <x/w/richtext/richtextcursorlocationfwd.H>
#include <x/w/richtext/richtextcursorlocationownerobj.H>
#include <x/w/richtext/richtextstringfwd.H>
#include <x/w/richtext/richtextfragmentfwd.H>
#include <x/w/rectangle.H>
#include <x/w/text_hotspot.H>
#include <x/obj.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! \ref richtextiterator "Rich text iterator" implementation object.

class richtextiteratorObj : public richtextcursorlocationownerObj {

public:

	//! My rich text object.
	const richtext my_richtext;

 public:

	friend class richtextObj; // It has access to my_location, I trust him.
	friend class richtext_implObj; // I trust him too
	friend struct richtext_insert_base; // Ditto

	//! Constructor

	//! Called by rich text's begin(), end(), and at() methods.

	richtextiteratorObj(const richtext &my_richtext,
			    const richtextcursorlocation &my_location,
			    richtextfragmentObj *my_fragment,
			    size_t offset,
			    new_location location_option);

	//! Alternative constructor used by clone()

	//! \internal
	richtextiteratorObj(const richtextiteratorObj &clone);

	//! Destructor
	~richtextiteratorObj();

	//! Create another iterator pointing to the beginning of the text.
	richtextiterator begin() const;

	//! Clone this iterator

	//! Creates another iterator for the same location
	richtextiterator clone() const;

	//! Swap two iterators
	void swap(const richtextiterator &other);

	//! Create another iterator pointing to the end of the text.
	richtextiterator end() const;

	//! Create another iterator pointing to a specific character
	richtextiterator pos(size_t offset) const;

	//! Move iterator left. Returns false if iterator was not moved.
	bool left(ONLY IN_THREAD);

	//! Move iterator right. Returns false if iterator was not moved.
	bool right(ONLY IN_THREAD);

	//! Adjust location to delete the character under the cursor.

	//! Called from the editor to advance the cursor in order to select
	//! the character under the cursor.
	void move_for_delete(ONLY IN_THREAD);

	//! Advance the iterator
	void move(ONLY IN_THREAD, ssize_t howmuch);

	//! Ditto
	void start_of_line();

	//! Ditto
	void end_of_line();

	//! Ditto
	void up(ONLY IN_THREAD);

	//! Ditto
	void down(ONLY IN_THREAD);

	//! Ditto
	void page_up(ONLY IN_THREAD, dim_t height);

	//! Ditto
	void page_down(ONLY IN_THREAD, dim_t height);

	//! Move to the given coordinates

	//! Returns false if the x coordinate is outside of the rendered
	//! text, true otherwise. The iterator gets positioned on the
	//! nearest character for the X coordinate, in all cases.
	bool moveto(ONLY IN_THREAD, coord_t x, coord_t y);

	//! What the at() returns.

	struct at_info {
		rectangle position; //!< Where the character is drawn.
		char32_t character; //!< The character at the iterator
		text_hotspotptr link; //!< The hotspot at the iterator
	};

	//! What's at this iterator's location
	at_info at(ONLY IN_THREAD) const;

	//! Current offset from the start of text, in characters
	size_t pos() const;

	//! Insert text at iterator location.

	//! The text is inserted before this iterator, the iterator remains
	//! pointing at its current location, just after the inserted text.
	//!
	//! Returns a iterator that points to the first character of the
	//! inserted text, and the number of inserted paragraphs, see
	//! richtextfragmentObj::insert().

	richtextiterator insert(ONLY IN_THREAD, richtextstring &&string);

	//! \overload

	richtextiterator insert(ONLY IN_THREAD,
				const std::u32string_view &string);

	//! Replace text.

	//! Like insert(), except replaces the text between this iterator
	//! and another one with new text. This iterator becomes the
	//! iterator to the end of the inserted text, returning the
	//! iterator

	void replace(ONLY IN_THREAD,
		     const const_richtextiterator &other,
		     richtextstring &&string,
		     bool replacing_hotspots) const;

	//! \overload

	void replace(ONLY IN_THREAD,
		     const const_richtextiterator &other,
		     const std::u32string_view &string) const;

 private:
	class internal_insert;
	template<typename> class internal_insert_impl;

	//! Internal implementation of insert().

	richtextiterator insert(ONLY IN_THREAD, const internal_insert &string);

	struct insert_lock;

	//! Internal implementation of insert()

	richtextiterator insert(ONLY IN_THREAD,
				struct insert_lock &lock_wrapper,
				const internal_insert &string);

	//! Internal implementation of replace()

	void replace(ONLY IN_THREAD,
		     const const_richtextiterator &other,
		     const internal_insert &string) const;

	//! Internal implementation of replace()

	void replace(ONLY IN_THREAD,
		     struct insert_lock &lock_wrapper,
		     const const_richtextiterator &other,
		     const internal_insert &string) const;

 public:

	//! Remove characters between this iterator and the other one

	void remove(ONLY IN_THREAD, const const_richtextiterator &other) const;

	//! Get text between the iterators

	//! Optionally overrides the richtext's bidi_format.
	//!
	//! \internal
	richtextstring
	get_richtextstring(const const_richtextiterator &other,
			   const std::optional<bidi_format> &embedding=
			   std::nullopt)
		const LIBCXX_HIDDEN;

	//! Count the number of characters that get_richtextstring will return.

	size_t count_richtextstring(const const_richtextiterator &other)
		const LIBCXX_HIDDEN;

	//! Compare iterator positions
	std::ptrdiff_t compare(const const_richtextiterator &other) const;

	//! Return current horizontal position

	dim_t horiz_pos(ONLY IN_THREAD);

	//! Set the "cursor" flag for this iterator

	void set_cursor(ONLY IN_THREAD, bool cursor_on);

	//! For testing purpose.

	auto &debug_get_location() const { return my_location; }
};

LIBCXXW_NAMESPACE_END

#endif
