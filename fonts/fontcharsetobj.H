/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef fontcharsetobj_H
#define fontcharsetobj_H

#include <x/ref.H>
#include <x/obj.H>
#include <vector>

#include <courier-unicode.h>
#include <x/functional.H>
#include "x/w/impl/fonts/fontcharsetfwd.H"

LIBCXXW_NAMESPACE_START

namespace fontconfig {
#if 0
}
#endif

//! A facade for the fontconfig library's internal FcCharset object.

class charsetObj : virtual public obj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	charsetObj(const ref<implObj> &implArg);

	//! Destructor
	~charsetObj();

	//! Add a character to the character set map.
	void add(char32_t);

	//! Remove a character to the character set map.
	void del(char32_t);

	//! Check if a character is in the character set map

	bool has(char32_t) const;

	//! Number of characters in the character set map.
	size_t count() const;

	//! Create a duplicate copy of this character set map.
	charset copy() const;

	//! Compare two character set maps.
	bool equal(const const_charset &cs) const;

	//! Create a new map that's intersection of the character set map.
	charset intersect(const const_charset &cs) const;

	//! Compute the size of an intersection of two character set maps.

	size_t intersect_count(const const_charset &cs) const;

	//! Check if this character set map is a subset of another map.
	bool is_subset(const const_charset &cs) const;

	//! Create a character set map that's a union of this and another map.
	charset union_of(const const_charset &cs) const;

	//! Subtract another map from this map, return a new map.
	charset subtract(const const_charset &cs) const;

	//! Compute the size of a subtraction of two character set maps.
	size_t subtract_count(const const_charset &cs) const;

	//! Combine another character set map with this one.
	void merge(const const_charset &cs) const;

	//! Callback passed to enumerate().

	//! The callable object gets repeatedly invoked to enumerate characters
	//! in this charset. The characters get enumerated in pages.
	//!
	//! Enumeration continues until all characters are enumerated, or until
	//! the callback function returns \c false.

	typedef bool enumerate_callback_t(const std::vector<char32_t> &chars);

	//! Enumerate character set map coverage.

	template<typename lambda>
		void enumerate(lambda &&l) const
	{
		do_enumerate(make_function<enumerate_callback_t>
			     (std::forward<lambda>(l)));
	}

	//! Enumerate character set map coverage.

	void do_enumerate(const function<enumerate_callback_t> &callback) const;
};

#if 0
{
#endif
}

LIBCXXW_NAMESPACE_END

#endif
