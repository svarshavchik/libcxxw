/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtext_insert_H
#define richtext_insert_H

#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtext_insertfwd.H"
#include "richtext/richtextcursorlocation.H"
#include <string>

LIBCXXW_NAMESPACE_START

//! Type erasure for richtextfragmentObj::insert()

//! Insert can insert either a richtextstring or a std::u32string. This
//! class type-erases the call to richtextstring::insert().
//!
//! This parameter to insert() also stores a richtextcursorlocation, which
//! insert() does not really use directly. insert() only calls pos() to
//! obtain the location in the fragment where the new string gets
//! insert()ed.
//!
//! richtext_insert_base gets constructed by richtextiteratorObj, which
//! owns the richtextcursorlocation, and gets forwarded to
//! richtext_implObj::insert_at_location(), which obtains the fragment
//! from the richtextcursorlocation, whose insert() gets finally invoked.

struct LIBCXX_HIDDEN richtext_insert_base {

	//! The location being inserted.

	const richtextcursorlocation location;

	//! Constructor

	richtext_insert_base(const richtextcursorlocation &location)
		: location{location}
	{
	}

	//! Invoke into's insert() method, to insert the string.

	virtual void operator()(richtextstring &into) const=0;

	//! Insert position
	inline auto pos() const
	{
		return location->get_offset();
	}

	//! Return the fragment being inserted.
	inline auto fragment() const
	{
		return location->my_fragment;
	}

	//! How many characters being inserted.
	virtual size_t size() const=0;
};

//! Implement richtext_insert_base for either a std::u32string or a richtextstring

template<typename type>
struct LIBCXX_HIDDEN richtext_insert : richtext_insert_base {

	//! The string being inserted.
	const type &s;

	//! Constructor
	richtext_insert(const richtextcursorlocation &location, const type &s)
		: richtext_insert_base{location}, s{s}
	{
	}

	//! Invoke richtextstring's insert() method.

	void operator()(richtextstring &into) const override
	{
		into.insert(pos(), s);
	}

	//! Return the string's size.
	size_t size() const override
	{
		return s.size();
	}
};

LIBCXXW_NAMESPACE_END

#endif
