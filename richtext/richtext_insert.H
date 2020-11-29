/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_insert_H
#define richtext_insert_H

#include "x/w/impl/richtext/richtextstring.H"
#include "richtext/richtextiterator.H"
#include "x/w/impl/richtext/richtext_insertfwd.H"
#include "x/w/impl/richtext/richtextcursorlocationfwd.H"
#include <x/functional.H>
#include <x/sorted_range.H>
#include <x/refptr_hash.H>
#include <string>
#include <vector>
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

	//! Constructor

	richtext_insert_base(const richtextiterator &iterator)
		: iterator{iterator}
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

	//! Insering a std::u32string_view
	static richtextstring get_richtext(const std::u32string_view &str,
					   const richtextmeta &default_meta)
	{
		std::unordered_map<size_t, richtextmeta> m{ {0, default_meta} };

		if (str.empty())
			m.clear();

		return richtextstring{str, m, false};
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
			type &s)
		: richtext_insert_base{iterator}, s{s}
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

	//! Constructor parameter
	richtextstring &string;

	//! Constructor parameter
	const unicode_bidi_level_t paragraph_embedding_level;

	size_t current_pos; //!< Next position to process

	const size_t n; //!< Cached string size.

public:
	//! Constructor
	create_fragments_from_inserted_text(richtextstring &string,
					    unicode_bidi_level_t
					    paragraph_embedding_level);

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

private:

	//! Find the start of next paragraph.
	size_t next_paragraph_start() const;
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

#if 0
	bool insert_rl=false;
#endif
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
};

LIBCXXW_NAMESPACE_END

#endif
