/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextiteratorobj_H
#define richtextiteratorobj_H

#include "x/w/impl/richtext/richtextiteratorfwd.H"
#include "x/w/impl/richtext/richtextfwd.H"
#include "richtext/richtextcursorlocation.H"
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtextfragmentfwd.H"
#include "x/w/rectangle.H"
#include "x/w/text_hotspot.H"
#include <x/obj.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! \ref richtextiterator "Rich text iterator" implementation object.

class richtextiteratorObj : virtual public obj {

public:

	//! My rich text object.
	const richtext my_richtext;

private:

	//! My cursor location object.

	//! An internal implementation object, basically. Accessible only
	//! while holding a lock on the underlying implementation object.

	richtextcursorlocation my_location;
 public:

	friend class richtextObj; // It has access to my_location, I trust him.

	//! Constructor

	//! Called by rich text's begin(), end(), and at() methods.

	richtextiteratorObj(const richtext &my_richtext,
			    const richtextcursorlocation &my_location,
			    richtextfragmentObj *my_fragment,
			    size_t offset);

	//! Alternative constructor used by clone()

	//! \internal
	richtextiteratorObj(const richtextiteratorObj &clone);

	//! Destructor
	~richtextiteratorObj() noexcept;

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

	//! Ahead by 1
	inline void next(ONLY IN_THREAD)
	{
		move(IN_THREAD, 1);
	}

	//! Reverse by 1
	inline void prev(ONLY IN_THREAD)
	{
		move(IN_THREAD, -1);
	}

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

	richtextiterator insert(ONLY IN_THREAD, const richtextstring &string);

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
		     const richtextstring &string) const;

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

	richtextstring get(const const_richtextiterator &other)
		const;

	//! Compare iterator positions
	std::ptrdiff_t compare(const const_richtextiterator &other) const;

	//! Return current horizontal position

	dim_t horiz_pos(ONLY IN_THREAD);

	//! Set the "cursor" flag for this iterator

	void set_cursor(ONLY IN_THREAD, bool cursor_on);

	//! For testing purpose.

	auto debug_get_location() const { return my_location; }
};

LIBCXXW_NAMESPACE_END

#endif
