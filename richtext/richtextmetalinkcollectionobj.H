/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef richtextmetalinkcollectionobj_H
#define richtextmetalinkcollectionobj_H

#include <x/obj.H>
#include <x/vectorobj.H>
#include "richtext/richtextstring.H"
#include "richtext/richtextmetalinkfwd.H"
#include "richtext/richtextmetalinkcollectionfwd.H"
#include <vector>

LIBCXXW_NAMESPACE_START

//! Tuples used by the \ref richtextmetalinkcollection "metadata link collections".

//! A tuple of <starting_position, count, link>

class metalink_tuple_t {

public:
	//! Starting range for this link.
	size_t starting_position;

	//! Number of characters spanned by the link.
	size_t character_count;

	//! The link
	richtextmetalink link;

	//! Constructor

	metalink_tuple_t(size_t starting_positionArg,
			size_t character_countArg,
			const richtextmetalink &linkArg);

	//! Destructor
	~metalink_tuple_t();
};

//! A helper class for applying metadata links.

//! Create a richtextstring in advance. To apply links to portions
//! of it, instantiate this factory, which inherits from a vector
//! of \ref metalink_tuple_t "tuples". Use emplace_back() to create them.
//!
//! Then invoke apply() to install the links into the
//! richtextmetamap_t.
//!
//! This class is not thread safe.

class richtextmetalinkcollectionObj
	: public vectorObj<metalink_tuple_t,
			   std::allocator<metalink_tuple_t>> {

public:

	//! Constructor
	richtextmetalinkcollectionObj();

	//! Destructor
	~richtextmetalinkcollectionObj();

	//! Apply the links to metadata.

	//! An exception gets thrown if the vector definition is
	//! inconsistent: duplicate links for the same starting position
	//! or link ranges overlap.

	void apply(richtextstring &string) const;
};

LIBCXXW_NAMESPACE_END

#endif
