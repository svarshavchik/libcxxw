/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_lock_h
#define x_w_input_field_lock_h

#include <x/ref.H>
#include <x/mpobj.H>
#include <x/refptr_traits.H>
#include <x/w/input_fieldfwd.H>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN richtext_implObj;

//! A container for an internally-implemented object.

typedef mpobj<ref<richtext_implObj>, std::recursive_mutex
	      > internal_richtext_impl_t;

//! Lock on a constant \ref input_field "input field"

//! \code
//! INSERT_LIBX_NAMESPACE::w::const_input_lock lock{f};
//!
//! std::string contents=lock.get();
//!
//! std::u32string contents=lock.get_unicode();
//! \endcode
//!
//! get() returns the current contents of the input field, in UTF-8; and
//! get_unicode() returns the current contents as a unicode string.
//!
//! get() and get_unicode() take an optional \ref bidi_format "bidi_format"
//! that overrides the \c directional_format configuration setting that was
//! specified when the input field was created.
//!
//! \code
//! size_t s=lock.size();
//!
//! std::tuple<size_t,size_t> pos=lock.pos();
//! \endcode
//!
//! size() returns the number of unicode characters in the input field,
//! and pos() returns the current cursor position and selection information.
//! The cursor position
//! consists of two values. The first value is the current cursor position,
//! with 0 denoting the start of the text. The second value is the same as
//! the first value when no portion of the text in the input field is
//! selected.
//!
//! A selection is defined by its starting position, and one past the
//! ending position. Subtracting the ending position from the starting
//! position gives the number of characters selected.
//!
//! When a selection exists, the first value returned by pos() is either
//! the starting or the ending position, whichever one the cursor is at.
//! The second value returned by pos() is the other position.
//! selection.
//!
//! \see input_lock

class const_input_lock : public internal_richtext_impl_t::lock {

public:

	//! Hold a reference on the input field

	const const_input_field locked_input_field;

	//! Constructor
	const_input_lock(const const_input_field &) LIBCXX_PUBLIC;

	//! Destructor

	~const_input_lock() LIBCXX_PUBLIC;

	//! Return the size of the input field.

	//! \note
	//! The returned size is in unicode characters.
	size_t size() const LIBCXX_PUBLIC;

	//! Return the current cursor position.

	//! Return the position of the cursor, and the position of the
	//! other end of the currently selected portion of the text.
	//! Both values are the same if there is no current selection.
	//!
	//! Both vaues are in unicode characters.
	std::tuple<size_t, size_t> pos() const LIBCXX_PUBLIC;

	//! Return the current contents of the input field as UTF-8.
	std::string get(const std::optional<bidi_format> &embedding=
			std::nullopt) const LIBCXX_PUBLIC;

	//! Return the current contents of the input field, as unicode.
	std::u32string get_unicode(const std::optional<bidi_format> &embedding=
				   std::nullopt) const LIBCXX_PUBLIC;

};

//! Lock the \ref input_field "input field" in order to access its contents.

//! Most functionality inherited from \ref const_input_lock "const_input_lock"
//! which implements non-mutating operations on the input field. With a mutable
//! input field provides a mutable \c locked_input_field to access the underlying
//! widget's mutable methods.
//!
//! \see const_input_lock

class input_lock : public const_input_lock {

 public:

	//! A mutable alias to the same input field.

	const input_field locked_input_field;

	//! Constructor
	input_lock(const input_field &) LIBCXX_PUBLIC;

	//! Destructor
	~input_lock() LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
