/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_w_impl_richtext_richtextstring_H
#define x_w_impl_richtext_richtextstring_H

#include <x/w/defaultthemefwd.H>
#include <x/w/richtext/richtextstringfwd.H>
#include <x/w/impl/richtext/richtextmetafwd.H>
#include <x/w/impl/fonts/freetypefontfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/types.H>
#include <courier-unicode.h>
#include <unordered_map>
#include <x/functional.H>
#include <optional>
#include <string_view>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Rich text direction

//! What's in the \ref richtextfragmentObj "rich text fragment".

enum class richtext_dir : unsigned char
	{
	 lr, //!< All normal left to right text.
	 rl, //!< All right to left text.
	 both, //!< Mixture of left to right and right to left text
	};

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

	//! The direction of this rich text string.

	//! This calculated by coalesce() too. It is out of date
	//! if coalesce_needed.
	mutable richtext_dir cached_dir=richtext_dir::lr;

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

	//! A lower_bound() search by the index # in the meta vector.
	meta_t::iterator meta_lower_bound_by_pos(size_t p);

public:
	//! An upper_bound() search by the index # in the meta vector.
	static meta_t::iterator meta_upper_bound_by_pos(meta_t &meta, size_t p);

	//! An upper_bound() search by the index # in the meta vector.
	static meta_t::const_iterator
	meta_upper_bound_by_pos(const meta_t &meta, size_t p);
private:
	//! Invoked by insert() and erase()
	void modified();

public:

	//! Default constructors
	richtextstring()=default;

	//! Constructor

	//! Initializes the richtextstring from the given string and metadata.

	template<typename string_type>
	richtextstring(string_type &&str,
		       const std::unordered_map<size_t, richtextmeta> &meta);

private:
	//! Finish the from string constructor.

	void finish_from_string();
public:

	//! Construct from a substring.

	richtextstring(const richtextstring &other,
		       size_t pos,
		       size_t n);

	//! Destructor
	~richtextstring();

	//! Clean up the internal structures
	void shrink_to_fit();

	//! Insert new text into the richtextstring

	void insert(size_t pos,
		    const std::u32string_view &s);

	//! Insert another string into richtextstring

	//! Specifies the insert point, and another string. Emphasis on
	//! "another", hence the parameter is passed by value :-).

	void insert(size_t pos, richtextstring s);
 private:

	//! insert() implementation.

	//! Must call modified() also.
	void do_insert(size_t pos,
		       const std::u32string_view &s,
		       meta_t &new_meta);
 public:

	//! Replace a portion of this string with another one.

	//! other_string's content, starting with its character \#other_pos
	//! and containing \#other_size characters replaces the contents of
	//! this string, starting at \#pos. This string must have at least
	//! pos+other_size characters.

	void replace(size_t pos,
		     const richtextstring &other_string,
		     size_t other_pos,
		     size_t other_size);

	//! Remove portions of this string.
	void erase(size_t pos, size_t n);

	//! Remove this entire string.
	void clear();

	//! Return the text string
	inline const std::u32string &get_string() const { return string; }

	//! Return the metadata.
	inline const meta_t &get_meta() const { coalesce(); return meta; }

	//! Comparator
	bool operator==(const richtextstring &s) const;

	//! Comparator
	inline bool operator!=(const richtextstring &s) const
	{
		return !operator==(s);
	}

	//! Return the direction of this text

	//! Returns cached indication whether this string contains only
	//! left to right text, right to left text, or both.

	inline richtext_dir &get_dir() const { coalesce(); return cached_dir; }

	//! Return the direction of this text

	//! Computes whether the given character range has only
	//! left to right text, right to left text, or both.

	richtext_dir get_dir(size_t from, size_t to) const;

	//! Compute this rich text string's unicode embedding level.

	//! Factor in the paragraph embedding level if this richtextstring
	//! has richtext_dir::both left-to-right and right-to-left content.
	//! Otherwise return the appropriate unicode embedding level value.

	unicode_bidi_level_t embedding_level(unicode_bidi_level_t
					     paragraph_embedding_level) const
	{
		switch (get_dir()) {
		case richtext_dir::lr:
			paragraph_embedding_level=UNICODE_BIDI_LR;
			break;
		case richtext_dir::rl:
			paragraph_embedding_level=UNICODE_BIDI_RL;
			break;
		case richtext_dir::both:
			break;
		}
		return paragraph_embedding_level;
	}

	//! Return the size of the string.
	inline size_t size() const { return string.size(); }

	//! Return metadata for the given offset.
	richtextmeta meta_at(size_t p);

 private:

	//! A container for resolved fonts.
	typedef std::vector<std::pair<size_t, freetypefont>> resolved_fonts_t;

	//! Container returned by resolve_fonts()

	//! resolve_fonts() is a const method. Fonts depends on the string
	//! and the meta.
	mutable resolved_fonts_t resolved_fonts;

	//! Whether the string or metadata was modified, and resolved_fonts needs caching.
	mutable bool fonts_need_resolving=true;

 public:
	//! Compute start of left-to-right text

	//! If the rich text string begins with right-to-left text, returns
	//! the starting index of the left to right content (or the size of
	//! the string if there are none).
	//!
	//! If the optional parameter is not null, also computes the iterator
	//! to the resolved font.

	size_t left_to_right_start(resolved_fonts_t::const_iterator *p=nullptr)
		const;

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

	const resolved_fonts_t &resolve_fonts() const;

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
	//! If \c next_string is not null it points to the "next"
	//! richtextstring. If the richtextstring dir is rl, the kerning
	//! of character 0 comes from the last font in the next richtextstring
	//! if it's the same as this font.
	//!
	//! embedding_level specifies the default paragraph bi-directional
	//! level, either UNICODE_BIDI_LR or UNICODE_BIDI_RL.
	//!
	//! Characters that cannot be represented get replaced by
	//! unprintable_char.

	void compute_width(const richtextstring *previous_string,
			   const richtextstring *next_string,
			   unicode_bidi_level_t embedding_level,
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

	void compute_width(const richtextstring *previous_string,
			   const richtextstring *next_string,
			   unicode_bidi_level_t embedding_level,
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

	struct LIBCXX_HIDDEN to_canonical_order;

	struct LIBCXX_HIDDEN from_canonical_order;

	struct LIBCXX_HIDDEN meta_reverse;

	//! Transformed hotspot marker character

	//! The begin and the end of a hotspot is initially marked by a line
	//! or a paragraph break, and is internally transformed to the
	//! hotspot_marker. The hotspot_marker should have no visual
	//! appearance, labels with hotspots should be using \0 as a
	//! replacement character for unprintable characters, which itself has
	//! no appearance. This character is picked so that it's unicode
	//! character type is UNICODE_BIDI_TYPE_S.

	static constexpr char32_t hotspot_marker=0x1F;
};

//! Convert a bidirectional string into one or more paragraphs

//! Each string is one paragraph from the original bidirectional string
//! in canonical rendering order. The paragraph break in a newline character.
//! For right-to-left rendering order it is the first character in the
//! canonical string.
//!
//! Each string is in canonical rendering order, and right-to-left text
//! gets marked with the rtol flag set in the metadata.

struct richtextstring::to_canonical_order {
	richtextstring &string;

	struct prepped_string;

	//! Constructor

	//! Takes a richtextstring and the requested canonical rendering
	//! order. The richtextstring is taken by reference, and will get
	//! scribbled all over.

	to_canonical_order(richtextstring &string,
			   const std::optional<unicode_bidi_level_t>
			   &paragraph_embedding_level);

private:
	//! Internal constructor

	to_canonical_order(prepped_string &&string,
			   const std::optional<unicode_bidi_level_t>
			   &paragraph_embedding_level);
public:
	//! Computed embedding level.

	//! Returns the canonical string's paragraph
	//! embedding level, same as the one passed to the constructor or the
	//! default one.

	inline auto paragraph_embedding_level() const
	{
		return std::get<1>(calc_results).direction;
	}
private:

	//! Calculated bidi types.
	unicode::bidi_calc_types types;

	//! Calculated bidi levels and paragraph embedding level.
	std::tuple<std::vector<unicode_bidi_level_t>,
		   struct unicode_bidi_direction> calc_results;

	//! Current canonical string's starting position
	size_t starting_pos;

	//! Number of characters in the canonical string.
	size_t n_chars;

	//! Starting position of the next canonical string.
	size_t ending_pos;

	//! Take starting_pos, compute n_chars and ending_pos

	void fill();

	//! Current paragraph as a canonical string (maybe)

	richtextstring canonical_string;

	//! Whether canonical_string has been filled.

	bool filled=false;

public:

	//! Whether operator* will return the next paragraph.

	//! After the last paragraph is advanced past, by opeerator++,
	//! this will return true.

	bool end()
	{
		fill();
		return n_chars == 0;
	}

	//! Return the current paragraph in canonical rendering order.

	//! The returned object remains valid until:
	//!
	//! 1) advance_to_next_paragraph gets called, and
	//! 2) current_paragraph() gets called again.
	//!
	//! The returned object does not get invalidated immediately by
	//! advance_to_next_paragraph(), but when this is called again,
	//! afterwards.

	richtextstring &current_paragraph()
	{
		fill();
		return canonical_string;
	}

	//! Advance to the next canonical paragraph.

	void advance_to_next_paragraph()
	{
		fill();

		if (!end())
		{
			filled=false;
			starting_pos=ending_pos;
		}
	}
};

//! Convert richtextstring from canonical to logical order

//! Constructing this object modifies the richtextstring that gets passed
//! to the constructor, from canonical to logical order.
//!
//! An optional subsequent call to embed() adds directional embedding markers
//! into the text, and returns it.
//!
//! embed_paragraph() checks that the given string's default paragraph embedding
//! level matches the specified one, and prepends a marker character if not.

struct richtextstring::from_canonical_order {
	richtextstring &string;
	const unicode_bidi_level_t paragraph_embedding;

	std::vector<unicode_bidi_level_t> levels;

	from_canonical_order(richtextstring &string,
			     unicode_bidi_level_t paragraph_embedding);

	richtextstring embed() const;

	static void embed_paragraph(richtextstring &string,
				    unicode_bidi_level_t paragraph_embedding);
};

#define x_w_impl_richtext_richtextstring_H_defined 1
#include <x/w/impl/richtext/richtextstringmeta.H>

LIBCXXW_NAMESPACE_END

#endif
