/*
** Copyright 2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "richtext/richtext_linebreak_info.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START
richtext_linebreak_info::richtext_linebreak_info(size_t skipArg,
						 size_t todoArg,
						 unicode_lb *ptrArg)
	: skip{skipArg},
	  todo{todoArg},
	  ptr{ptrArg},
	  meta_index{0},
	  rl{false}
{
}

richtext_linebreak_info::~richtext_linebreak_info()=default;

int richtext_linebreak_info::callback(int intvalue)
{
	switch (intvalue) {
	case UNICODE_LB_NONE:
	case UNICODE_LB_ALLOWED:
	case UNICODE_LB_MANDATORY:
		break;
	default:
		throw EXCEPTION("Internal error: unknown linebreaking value");
	}

	auto value=static_cast<unicode_lb>(intvalue);

	// Consume one char's worth of metadata.
	//
	// We keep track of each char's metadata, as we
	// receive each char's line break status, here.
	//
	// When encountering a change of direction, make
	// sure that linebreaks are allowed, here.

	if (metadata.empty())
		throw EXCEPTION("Unexpected end of metadata "
				"while calculating line breaks"
				);

	auto &[meta_b, meta_e, string_size]=metadata.front();
	if (meta_index == 0)
	{
		if (meta_b->second.rl != rl &&
		    value != unicode_lb::mandatory)
			value=unicode_lb::allowed;

		rl=meta_b->second.rl;
	}

	if (++meta_index >=
	    (meta_b+1 == meta_e ? string_size:
	     meta_b[1].first)-meta_b->first)
	{
		if (++meta_b == meta_e)
			metadata.pop();
		meta_index=0;
	}

	if (skip)
	{
		--skip;
		return 0;
	}

	if (todo)
	{
		*ptr++=value;
		--todo;
	}
	return 0;
}

void richtext_linebreak_info::operator()(const richtextstring &s)
{
	auto &string=s.get_string();

	if (!string.empty())
	{
		const auto &meta=s.get_meta();

		auto b=meta.begin(), e=meta.end();

		if (b == e)
			throw EXCEPTION("Invalid text metadata "
					"during line break "
					"calculation");
		metadata.emplace(b, e, string.size());
	}
	unicode::linebreak_callback_base::operator()
		(string.begin(), string.end());
}

LIBCXXW_NAMESPACE_END
