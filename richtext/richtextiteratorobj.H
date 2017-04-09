/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextiteratorobj_H
#define richtextiteratorobj_H

#include "richtext/richtextiteratorfwd.H"
#include "richtext/richtextfwd.H"
#include "richtext/richtextcursorlocationfwd.H"
#include "richtext/richtextstring.H"
#include "x/w/rectangle.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

class richtextfragmentObj;

//! \ref richtextiterator "Rich text iterator" implementation object.

class LIBCXX_HIDDEN richtextiteratorObj : virtual public obj {

 public:

	//! My rich text object.
	const richtext my_richtext;

 private:

	//! My cursor location object.

	//! An internal implementation object, basically. Accessible only
	//! while holding a lock on the underlying implementation object.

	const richtextcursorlocation my_location;
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

	//! Create another iterator pointing to the end of the text.
	richtextiterator end() const;

	//! Create another iterator pointing to a specific character
	richtextiterator pos(size_t offset) const;

	//! Ahead by 1
	inline void next(IN_THREAD_ONLY)
	{
		move(IN_THREAD, 1);
	}

	//! Reverse by 1
	inline void prev(IN_THREAD_ONLY)
	{
		move(IN_THREAD, -1);
	}

	//! Advance the iterator
	void move(IN_THREAD_ONLY, ssize_t howmuch);

	//! Ditto
	void start_of_line();

	//! Ditto
	void end_of_line();

	//! Ditto
	void up(IN_THREAD_ONLY);

	//! Ditto
	void down(IN_THREAD_ONLY);

	//! What the at() returns.

	struct at_info {
		rectangle position; //!< Where the character is drawn.
		char32_t character; //!< The character at the iterator
	};

	//! What's at this iterator's location
	at_info at(IN_THREAD_ONLY) const;

	//! Current offset from the start of text, in characters
	size_t pos() const;

	//! Insert text at iterator location.

	//! The text is inserted before this iterator, the iterator remains
	//! pointing at its current location, just after the inserted text.
	//!
	//! Returns a iterator that points to the first character of the
	//! inserted text, and the number of inserted paragraphs, see
	//! richtextfragmentObj::insert().

	std::pair<richtextiterator, size_t>
		insert(IN_THREAD_ONLY, const richtextstring &string);

	//! \overload

	std::pair<richtextiterator, size_t>
		insert(IN_THREAD_ONLY, const std::u32string &string);

 private:
	class internal_insert;
	template<typename> class internal_insert_impl;

	//! Internal implementation of insert().

	std::pair<richtextiterator, size_t>
		insert(IN_THREAD_ONLY, const internal_insert &string);
 public:

	//! Remove characters between this iterator and the other one

	void remove(IN_THREAD_ONLY, const const_richtextiterator &other) const;

	//! Get text between the iterators

	richtextstring get(const const_richtextiterator &other)
		const;

	//! Compare iterator positions
	std::ptrdiff_t compare(const const_richtextiterator &other) const;

	//! Return current horizontal position

	dim_t horiz_pos(IN_THREAD_ONLY);

	//! Set the "cursor" flag for this iterator

	void set_cursor(IN_THREAD_ONLY, bool cursor_on);

	//! For testing purpose.

	auto debug_get_location() const { return my_location; }
};

LIBCXXW_NAMESPACE_END

#endif
