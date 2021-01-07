/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/uixmlparser.H"

LIBCXXW_NAMESPACE_START

ui::parser_lock::parser_lock(const xml::readlock &l)
	: xml::readlock{l}, c_locale{locale::create("C")}
{
}

ui::parser_lock::parser_lock(const xml::readlock &l,
			     const const_locale &c_locale)
	: xml::readlock{l}, c_locale{c_locale}
{
}

ui::parser_lock::~parser_lock()=default;

ui::parser_lock ui::parser_lock::clone() const
{
	return parser_lock{
		xml::readlock::operator->()->clone(), c_locale };
}

LIBCXXW_NAMESPACE_END
