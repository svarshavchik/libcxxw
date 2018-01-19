/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "theme_parser_lock.H"

LIBCXXW_NAMESPACE_START

theme_parser_lock::theme_parser_lock(const xml::doc::base::readlock &l,
				     const const_locale &c_locale)
	: xml::doc::base::readlock{l}, c_locale{c_locale}
{
}

theme_parser_lock::~theme_parser_lock()=default;

theme_parser_lock theme_parser_lock::clone() const
{
	return theme_parser_lock{
		xml::doc::base::readlock::operator->()->clone(),
			c_locale };
}

LIBCXXW_NAMESPACE_END
