/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/uigenerators.H"
#include "uicompiler.H"

#include "theme_parser_lock.H"
#include <x/xml/doc.H>

LIBCXXW_NAMESPACE_START

const_uigenerators uigeneratorsBase::create(const std::string_view &filename)
{
	auto xml=xml::doc::create(filename, "nonet xinclude");

	auto g=ptrref_base::objfactory<uigenerators>::create();

	theme_parser_lock lock{xml->readlock()};

	if (lock->get_root())
	{
		uicompiler compiler{lock, *g, true};
	}

	return g;
}

uigeneratorsObj::uigeneratorsObj()=default;

uigeneratorsObj::~uigeneratorsObj()=default;

LIBCXXW_NAMESPACE_END
