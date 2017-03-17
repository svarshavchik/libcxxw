/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtextmetalinkcollection.H"
#include "assert_or_throw.H"

LIBCXXW_NAMESPACE_START

metalink_tuple_t::metalink_tuple_t(size_t starting_positionArg,
				   size_t character_countArg,
				   const richtextmetalink &linkArg)
	: starting_position(starting_positionArg),
	  character_count(character_countArg),
	  link(linkArg)
{
}

metalink_tuple_t::~metalink_tuple_t()=default;

richtextmetalinkcollectionObj::richtextmetalinkcollectionObj()=default;

richtextmetalinkcollectionObj::~richtextmetalinkcollectionObj()=default;

void richtextmetalinkcollectionObj::apply(richtextstring &string) const
{
	// Sort by starting position

	std::map<size_t, const metalink_tuple_t *> lookup;

	auto s=string.get_string().size();

	for (const auto &tuple:*this)
	{
		assert_or_throw(tuple.starting_position < s &&
				tuple.character_count <=
				s - tuple.starting_position,
				"Rich text metadata link exceeds text size");

		assert_or_throw(lookup.insert
				(std::make_pair(tuple.starting_position,
						&tuple)).second,
				"Duplicate rich text metadata link position");
	}

	// Another sanity check

	size_t starting_pos=0;

	for (const auto &l:lookup)
	{
		assert_or_throw(l.first >= starting_pos,
				"Overlapping rich text metadata ranges");

		starting_pos=l.first+l.second->character_count;
	}

	// Update the meta map

	for (const auto &l:lookup)
	{
		if (l.second->character_count == 0)
			continue;

		string.modify_meta(l.first, l.second->character_count,
				   [&]
				   (size_t ignore,
				    richtextmeta &meta)
				   {
					   meta.link=l.second->link;
				   });
	}
}

LIBCXXW_NAMESPACE_END
