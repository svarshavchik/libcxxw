/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/uixmlparser.H"

LIBCXXW_NAMESPACE_START

ui::parser_lock::parser_lock(const xml::readlock &l)
	: xml::readlock{l}
{
}

ui::parser_lock::~parser_lock()=default;

ui::parser_lock ui::parser_lock::clone() const
{
	return parser_lock{
		xml::readlock::operator->()->clone()
	};
}

LIBCXXW_NAMESPACE_END
