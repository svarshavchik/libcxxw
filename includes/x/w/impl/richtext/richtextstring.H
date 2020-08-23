/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextstring_H
#define x_w_impl_richtext_richtextstring_H

#include <x/w/defaultthemefwd.H>
#include <x/w/impl/richtext/richtextstringfwd.H>
#include <x/w/impl/richtext/richtextmetafwd.H>
#include <x/w/impl/fonts/freetypefontfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/types.H>
#include <courier-unicode.h>
#include <unordered_map>
#include <x/functional.H>
#include <string_view>

LIBCXXW_NAMESPACE_START

//! A unicode string, marked up by \ref richtextmeta "a richtextmeta".

//! Encapsulate a unicode string, with a list of fonts and colors for drawing
//! it.
//!
//! Define a \c std::u32string, and a list of
//! index+richtextmeta tuples. "index" specifies that character \#n in the
//! \c std::u32string, and all following characters up until th enext "index"
//! are rendered by the corresponding \c richtextmeta.
//!
//! Index #0 must always be specified, unless the string is empty.
//! The last index in the list of index+richtextmeta tuples specifies the
//! metadata for the remainder of the text string.
//!
//! This class is not thread safe. It's expected that it's sitting behind
//! a lock, somewhere. Some methods are accessible only to the execution
//! thread, but the lock must still be acquired before invoking them.

class richtextstring {

	//! Our unicode string.
	std::u32string string;

public:
	//! Our container for the metadata.

	//! This a tuple of index+richtextmeta, sorted by index. Externally
	//! this is sorced from a map, so there won't be any duplicates.
	typedef std::vector<std::pair<size_t, richtextmeta>> meta_t;
private:
	//! The metadata.

	//! coalesce() updates this, but it's a const method.
	mutable meta_t meta;

	//! The text string is in rendering order.

	bool is_render_order=false;

	//! Sort the meta, which was imported from a map.

	//! Sorts it by index, validates it against a stirng of size 'n'.
	//!
	//! If 'n' is 0, the metadata must be empty. Otherwise the last index
	//! in the metadata must be less than 'n'. Since the metadata comes
	//! from a map there won't be duplicates. There must be an entry
	//! for index #0.

	static void sort_and_validate(meta_t &meta, size_t n);

	//! Whether a coalesce() is needed.

	//! This flag is set when the string or metadata is modified, and
	//! is cleared by coalesce().
	//! coalesce() updates this, but it's a const method.

	mutable bool coalesce_needed=false;

	//! Remove consecutive metadata entries that are the same.

	//! If \c coalesce_needed is set, scans the metadata array.
	//! Consecutive entries with the same metadata are removed.
	void coalesce() const;

	//! Make sure there is an entry in the metadata vector for index \#n.

	//! If there isn't one, duplicate the next lowest one, just before
	//! index \#n.
	meta_t::iterator duplicate(size_t);

	//! Make sure there is an entry in the metadata vector for index \#n.

	//! If there isn't one, duplicate the next lowest one, just before
	//! index \#n.
	static meta_t::iterator duplicate(meta_t &, size_t);

	class compare_meta_by_pos;

	//! A lower_bound() search by the index # in the meta vector.
	meta_t::iterator meta_lower_bound_by_pos(size_t p);

	//! An upper_bound() search by the index # in the meta vector.
	static meta_t::iterator meta_upper_bound_by_pos(meta_t &meta, size_t p);

	//! Invoked by insert() and erase()
	void modified();

	//! Invoked by render_order() and logical_order()

	void swap_order();

public:

	//! Default constructors
	richtextstring()=default;

	//! Constructor

	//! Initializes the richtextstring from the given string and metadata.

	richtextstring(const std::u32string &string,
		       const std::unordered_map<size_t, richtextmeta> &meta,
		       bool append_null_byte=false);

	//! Construct from a substring.

	richtextstring(const richtextstring &other,
		       size_t pos,
		       size_t n);

	//! Destructor
	~richtextstring();

	//! Insert new text into the richtextstring

	//! Specifies the insert point, and another string.

	void insert(size_t pos,
		    const std::u32string_view &s);

	//! Insert another string into richtextstring

	//! Specifies the insert point, and another string. Emphasis on
	//! "another", hence the parameter is passed by value :-).

	void insert (size_t pos, richtextstring s);
 private:

	//! insert() implementation.

	//! Must call modified() also.
	void do_insert(size_t pos,
		       const std::u32string_view &s,
		       meta_t &new_meta);
 public:
	//! We want to see the characters in logical order

	inline void logical_order()
	{
		if (is_render_order)
			swap_order();
	}

	//! We want to see the characters in rendering order

	inline void render_order()
	{
		if (!is_render_order)
			swap_order();
	}

	//! Remove portions of this string.
	void erase(size_t pos, size_t n);

	//! Remove this entire string.
	void clear();

	//! Return the text string
	inline const std::u32string &get_string() const { return string; }

	//! Return the metadata.
	inline const meta_t &get_meta() const { coalesce(); return meta; }

	//! Return the size of the string.
	inline size_t size() const { return string.size(); }

	//! Return metadata for the given offset.
	richtextmeta meta_at(size_t p);

 private:

	//! A container for resolved fonts.
	typedef std::vector<std::pair<size_t, freetypefont>> resolved_fonts_t;

	//! Container returned by resolve_fonts()

	resolved_fonts_t resolved_fonts;

	//! Whether the string or metadata was modified, and resolved_fonts needs caching.
	bool fonts_need_resolving=true;

 public:
	//! Whether font resolution is required

	bool need_font_resolution() const
	{
		return fonts_need_resolving;
	}

	//! Lookup the metadata's freetypefonts.

	//! Determine consecutive ranges of this text string that get rendered
	//! with the same freetypefont.
	//!
	//! Returns a vector containing:
	//!
	//! - starting character index
	//!
	//! - freetypefont for rendering text starting at the given
	//! character.
	//!
	//! The vector is sorted by starting position. For a non-empty string
	//! the first vector always has index #0.
	//! The freetypefont remains in effect until the next element's index,
	//! or the end of the string.

	const resolved_fonts_t &resolve_fonts();

	//! Calculate the width and the kerning of each character.

	//! Computes the width of each character, in pixels. The current is
	//! the offset to be added to the previous character's position plus
	//! its width to get the starting position of this character.
	//!
	//! If \c previous_string is not null it points to a "previous"
	//! richtextstring. The kerning of character #0 is zero, unless
	//! the last font in the previous richtextstring is the same as the
	//! first font in this string.
	//!
	//! Characters that cannot be represented get replaced by
	//! unprintable_char.

	void compute_width(richtextstring *previous_string,
			   char32_t unprintable_char,
			   std::vector<dim_t> &widths,
			   std::vector<int16_t> &kernings);

	//! Update existing widths and kernings.

	//! This is a timesaver for updating an existing width and kernings
	//! vector for the given character range.
	//!
	//! "skip" gives the index of the first character whose width and
	//! kerning gets updated.
	//!
	//! "count" gives the count of characters, starting at "skip", that
	//! get updated.

	void compute_width(richtextstring *previous_string,
			   char32_t unprintable_char,
			   std::vector<dim_t> &widths,
			   std::vector<int16_t> &kernings,
			   size_t skip,
			   size_t count);

	//! Reserve space for so many characters and metadata.

	void reserve(size_t n_chars, size_t n_meta);

	//! An append operation

	richtextstring &operator+=(const richtextstring &o);

	//! The current theme was updated.

	//! Invoke all fonts' theme_updated().
	void theme_updated(ONLY IN_THREAD,
			   const const_defaulttheme &new_theme);

	//! Modify rich text meta.

	//! Invoke the lambda, passing it all the metadata within the given
	//! range.
	template<typename lambda>
		void modify_meta(size_t start, size_t count,
				 lambda &&l)
	{
		do_modify_meta(start, count,
			       make_function<void (size_t, richtextmeta &meta)>
			       (std::forward<lambda>(l)));
	}

	//! Modify rich text meta.

	//! Invoke the lambda, passing it all the metadata within the given
	//! range.
	void do_modify_meta(size_t start, size_t count,
			    const function<void (size_t, richtextmeta &meta)>
			    &l);
};

LIBCXXW_NAMESPACE_END

#endif
