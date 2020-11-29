/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_insert_H
#define richtext_insert_H

#include "x/w/impl/richtext/richtextstring.H"
#include "richtext/richtextiterator.H"
#include "x/w/impl/richtext/richtext_insertfwd.H"
#include <x/functional.H>
#include "richtext/richtextcursorlocation.H"
#include <string>
#include <vector>
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

class LIBCXX_HIDDEN create_fragments_from_inserted_text;

//! Create fragments out of inserted text.

//! One fragment per paragraph. The first step of processing inserted text
//! is to break it up into one paragraph per fragment.
//!
//! Takes a richtextstring and each call to operator() produces a
//! richtextfragment. Or a null pointer after all fragments were produced.

class create_fragments_from_inserted_text {

	//! Constructor parameter
	richtextstring &string;

	//! Constructor parameter
	const unicode_bidi_level_t embedding_level;

	//! Cached line breaks
	std::vector<unicode_lb> breaks;

	size_t current_pos; //!< Next position to process

	const size_t n; //!< Cached breaks.size()

public:
	//! Constructor
	create_fragments_from_inserted_text(richtextstring &string,
					    unicode_bidi_level_t
					    embedding_level);

	//! Destructor
	~create_fragments_from_inserted_text();

	//! Return next fragment.
	richtextfragmentptr operator()();
};


LIBCXXW_NAMESPACE_END

#endif
