/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/bookpagefactory.H"
#include "x/w/shortcut.H"
#include "x/w/factory.H"
#include "x/w/label.H"

LIBCXXW_NAMESPACE_START

bookpagefactoryObj::bookpagefactoryObj()=default;


bookpagefactoryObj::~bookpagefactoryObj()=default;

void bookpagefactoryObj::do_add(const function<void (const factory &,
						     const factory &)> &f)
{
	do_add(f, {});
}

void bookpagefactoryObj::do_add(const text_param &label,
				const function<void (const factory &)> &f,
				LIBCXXW_NAMESPACE::halign h)
{
	do_add(label, f, {}, h);
}

void bookpagefactoryObj::do_add(const text_param &label,
				const function<void (const factory &)> &f,
				const shortcut &shortcut,
				LIBCXXW_NAMESPACE::halign h)
{
	add([&]
	    (const factory &label_factory,
	     const factory &page_factory)
	    {
		    label_factory->create_label(label, h)->show();
		    f(page_factory);
	    }, shortcut);
}

LIBCXXW_NAMESPACE_END
