/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/uixmlparser.H"

LIBCXXW_NAMESPACE_START

ui::parser_lock::parser_lock(const xml::doc::base::readlock &l)
	: xml::doc::base::readlock{l}, c_locale{locale::create("C")}
{
}

ui::parser_lock::parser_lock(const xml::doc::base::readlock &l,
				     const const_locale &c_locale)
	: xml::doc::base::readlock{l}, c_locale{c_locale}
{
}

ui::parser_lock::~parser_lock()=default;

ui::parser_lock ui::parser_lock::clone() const
{
	return parser_lock{
		xml::doc::base::readlock::operator->()->clone(),
			c_locale };
}

LIBCXXW_NAMESPACE_END
