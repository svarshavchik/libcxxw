/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_insert_H
#define richtext_insert_H

#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/richtext/richtextiterator.H"
#include "x/w/richtext/richtext_insertfwd.H"
#include "x/w/richtext/richtextcursorlocationfwd.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "richtext/richtextcursorlocation.H"
#include <x/functional.H>
#include <x/sorted_range.H>
#include <x/refptr_hash.H>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

//! Type erasure for richtextfragmentObj::insert()

//! Insert can insert either a richtextstring or a std::u32string. This
//! class type-erases the call to richtextstring::insert().
//!
//! This parameter to insert() also stores a richtextiterator, which
//! insert() does not really use directly. insert() only calls pos() to
//! obtain the location in the fragment where the new string gets
//! insert()ed.
//!
//! richtext_insert_base gets constructed by richtextiteratorObj, which
//! owns the richtextcursorlocation, and gets forwarded to
//! richtext_implObj::insert_at_location(), which obtains the fragment
//! from the richtextcursorlocation, whose insert() gets finally invoked.

struct richtext_insert_base {

	//! The location being inserted.

	const richtextiterator iterator;

	//! Whether twe are replacing an existing hotspot

	const bool replacing_hotspots;

	//! Constructor

	richtext_insert_base(const richtextiterator &iterator,
			     bool replacing_hotspots)
		: iterator{iterator}, replacing_hotspots{replacing_hotspots}
	{
	}

	//! Return the string to insert

	//! Receives the metadata at the insert position. This is used to
	//! construct a richtextstring from a std::u32string_view.
	virtual richtextstring operator()(const richtextmeta &default_meta)
		const=0;

	//! Inserting another richtextstring.
	static richtextstring get_richtext(richtextstring &str,
					   const richtextmeta &default_meta)
	{
		return std::move(str);
	}

	//! Inserting a std::u32string_view
	static richtextstring get_richtext(const std::u32string_view &str,
					   richtextmeta default_meta)
	{
		// default_meta is the current meta at the position. Clear its
		// right-to-left flag, this will be the metadata for the
		// inserted unicode string.

		default_meta.rl=false;

		std::unordered_map<size_t, richtextmeta> m{ {0, default_meta} };

		if (str.empty())
			m.clear();

		auto direction=unicode_bidi_get_direction(str.data(),
							  str.size());

		// Return the string as if:
		//
		// 1) it has a character with an explicit direction or is
		//    empty
		//
		// 2) or it has an override.

		if (direction.is_explicit || str.empty() ||
		    std::find_if(str.begin(), str.end(),
				 []
				 (char32_t c)
				 {
					 return c == UNICODE_LRO ||
						 c == UNICODE_RLO;
				 }) != str.end())
			return richtextstring{str, m};

		// We must be inserting text with implicit direction, however
		// we need to make it explicit, by explicitly adding an
		// override.

		std::u32string directed_string;

		directed_string.reserve(str.size()+2);

		directed_string.push_back(direction.direction == UNICODE_BIDI_LR
					  ? UNICODE_LRO:UNICODE_RLO
					  );
		directed_string += str;
		directed_string += UNICODE_PDF;

		return richtextstring{std::move(directed_string), m};

	}

	//! Insert position
	inline auto pos() const
	{
		return iterator->my_location->get_offset();
	}

	//! Return the fragment being inserted.
	inline auto fragment() const
	{
		return iterator->my_location->my_fragment;
	}

	//! How many characters being inserted.
	virtual size_t size() const=0;
};

//! Implement richtext_insert_base for either a std::u32string or a richtextstring

template<typename type>
struct LIBCXX_HIDDEN richtext_insert : richtext_insert_base {

	//! The string being inserted.
	type &s;

	//! Constructor
	richtext_insert(const richtextiterator &iterator,
			bool replacing_hotspots,
			type &s)
		: richtext_insert_base{iterator, replacing_hotspots}, s{s}
	{
	}

	richtextstring operator()(const richtextmeta &default_meta)
		const override
	{
		return this->get_richtext(this->s, default_meta);
	}

	//! Return the string's size.
	size_t size() const override
	{
		return s.size();
	}
};

//! Create fragments out of inserted text.

//! One fragment per paragraph. The first step of processing inserted text
//! is to break it up into one paragraph per fragment.
//!
//! Takes a richtextstring and each call to operator() produces a
//! richtextfragment. Or a null pointer after all fragments were produced.
//!
//! \note
//! The constructor modifies the passed in string. All newlines are updated
//! to have their text direction reflect the paragraph_embedding_level.

class create_fragments_from_inserted_text {

	//! Allt the hard work happens here.
	mutable richtextstring::to_canonical_order canonicalizer;

public:
	//! Constructor
	create_fragments_from_inserted_text(richtextstring &string,
					    const std::optional
					    <unicode_bidi_level_t> &
					    requested_embedding_level,
					    bool replacing_hotspots,
					    unicode_bidi_level_t
					    &paragraph_embedding_level);

	//! Destructor
	~create_fragments_from_inserted_text();

	//! Return next string for a fragment.

	//! An empty string gets returned if there's no more.
	richtextstring next_string();

	//! Returns the direction of the next_string()

	//! Called before next_string() is called.
	richtext_dir next_string_dir() const;

	//! Peek at whether we have a paragraph break, or not.

	bool has_paragraph_break() const;

	//! Whether we've reached the end.
	bool end() { return canonicalizer.end(); }
};

//! What happened in insert()

//! insert() returns this object that reports metadata of what was inserted.
//! This gets passed along to richtextfragmentObj::split and
//! richtextfragmentObj in order to rewrap the inserted text. They invoke
//! richtext_insert_results::split() and
//! richtext_insert_results::merged() to, basically, log their action.

struct richtext_insert_results {

	//! Cursor locations that should be moved as a result of the new text.

	std::vector<richtextcursorlocation> cursors_to_move;

	//! Range of inserted text

	//! The starting and ending position of inserted text in a fragment.

	struct range {
		size_t begin;
		size_t end;
	};

	//! Where text has been inserted

	//! Keeps track of where the inserted text is. This is a map
	//! of richtextfragments. insert() inserts the text and records
	//! each richtextfragment it updated or created. For each
	//! richtextfragment a starting and ending position of the inserted
	//! text gets recorded.
	//!
	//! Then, this gets updated by split() and merged() as the inserted
	//! text gets rewrapped.
	//!
	//! The starting and ending position is a slight overkill, since
	//! we reasonably expect to keep track of at most one range per
	//! fragment.

	std::unordered_map<richtextfragment,
			   sorted_range<range>> inserted_range;

	//! We did an insert of right-to-left text.
	bool insert_rl=false;

	//! Whether the inserted text ended in a paragraph break

	//! This is to let insert_at_location() to know that it needs to
	//! rewrap an additional paragraph, since the inserted text ends
	//! on the last line of the insert-ending paragraph, but the following
	//! paragraph also needs to get rewrapped.
	bool insert_ended_in_paragraph_break=false;

	//! Fragments were split.

	//! The range of character specifies by pos (offset) and count (number
	//! of characters) was removed from the split_from fragment and
	//! placed into a new split_to fragment.
	//!
	//! This updates inserted_range accordingly.
	void split(const richtextfragment &split_from,
		   size_t pos,
		   size_t count,
		   const richtextfragment &split_to);

	//! Fragments were merged.

	//! merged_from was the fragment whose contents were merged into
	//! the merged_to fragment, starting at the given position.
	//!
	//! This updates inserted_range accordingly.
	void merged(const richtextfragment &merged_from,
		    const richtextfragment &merged_to,
		    size_t merged_to_pos);

	//! An entire fragment was removed.
	void removed(const richtextfragment &f);

	//! An entire fragment was removed.
	void removed(const richtextfragment &f,
		     size_t pos, size_t nchars);

	//! Insert complete, move cursors according to the inserted_range
	void move_cursors();
};

LIBCXXW_NAMESPACE_END

#endif
