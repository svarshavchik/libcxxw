/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_lock_h
#define x_w_input_field_lock_h

#include <x/ref.H>
#include <x/exception.H>
#include <x/mpobj.H>
#include <x/w/input_fieldfwd.H>
#include <x/w/richtext/richtextiteratorfwd.H>
#include <x/w/text_param.H>
#include <tuple>
#include <optional>

LIBCXXW_NAMESPACE_START

//! Internal implementation object.

class LIBCXX_HIDDEN richtext_implObj;

//! Internal implementation object.

class LIBCXX_HIDDEN editor_implObj;

//! A container for an internally-implemented object.

//! Acquiring a lock on this object blocks access to the internal
//! richtext structure that represents the contents of the input field,
//! blocking other execution threads from accessing it

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

class const_input_lock : private internal_richtext_impl_t::lock {

protected:
	const ref<editor_implObj> internal_editor;

public:
	//! Constructor
	const_input_lock(const const_input_field &) LIBCXX_PUBLIC;

	//! Internal constructor
	const_input_lock(const ref<editor_implObj> *) LIBCXX_HIDDEN;

	//! Destructor

	~const_input_lock() LIBCXX_PUBLIC;

	//! Return the size of the input field.

	//! \note
	//! The returned size is in unicode characters.
	size_t size() const LIBCXX_PUBLIC;

	//! Return the current cursor position.

	//! \code
	//! #include <INSERT_LIBX_NAMESPACE/w/richtext/richtextiterator.H>
	//! \endcode
	//!
	//! Return an iterator to the cursor, and an iterator to the
	//! other end of the currently selected portion of the text.
	//! Both iterators are the same if there is no current selection.
	//!
	//! \note
	//! The second iterator can be before the first iterator, this
	//! depends on the order the portion of the text was selected.
	//!
	//! \note
	//! The connection thread can update the underlying contents and the
	//! cursor positions at any given time, but is blocked from doing so
	//! by this input lock. The first iterator, the current cursor position
	//! remains valid after this input lock goes out of scope and gets
	//! destroyed. The second iterator too, but will no longer carry any
	//! particular meaning.

	std::tuple<const_richtextiterator, const_richtextiterator
			   > pos() const LIBCXX_PUBLIC;

	//! Return the current contents of the input field.
	std::string get(const std::optional<bidi_format> &embedding=
			std::nullopt) const LIBCXX_PUBLIC;

	//! Return the current contents of the input field, as unicode.
	std::u32string get_unicode(
		const std::optional<bidi_format> &embedding=std::nullopt)
		const LIBCXX_PUBLIC;

	//! Forwarded to stop_message() in the main_window.

	void stop_message(const text_param &) const LIBCXX_PUBLIC;

	//! Forwarded to exception_message() in the main_window.

	void exception_message(const exception &) const LIBCXX_PUBLIC;
};

//! \anchor input_lock Lock the \ref input_field "input field" in order to access its contents.

//! Most functionality inherited from \ref const_input_lock "const_input_lock"
//! which implements non-mutating operations on the input field. With a mutable
//! input field provides a mutable \c locked_input_field to access the underlying
//! widget's mutable methods.
//!
//! \see const_input_lock

class input_lock : public const_input_lock {

 public:

	using const_input_lock::const_input_lock;

	//! Destructor
	~input_lock() LIBCXX_PUBLIC;

	//! Return the current cursor position.

	//! \see const_input_field_editor
	std::tuple<richtextiterator, richtextiterator> pos()
		LIBCXX_PUBLIC;

	//! Set the contents of the input field.
	void set(const std::string_view &str, bool validated=false)
		LIBCXX_PUBLIC;

	//! Set the contents of the input field, as unicode.
	void set(const std::u32string_view &str, bool validated=false)
		LIBCXX_PUBLIC;

	//! Set the contents of the input field.
	void set(ONLY IN_THREAD,
		 const std::string_view &str, bool validated=false)
		LIBCXX_PUBLIC;

	//! Set the contents of the input field, as unicode.
	void set(ONLY IN_THREAD,
		 const std::u32string_view &str, bool validated=false)
		LIBCXX_PUBLIC;
};

LIBCXXW_NAMESPACE_END

#endif
