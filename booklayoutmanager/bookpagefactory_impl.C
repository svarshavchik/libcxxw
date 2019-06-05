/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/bookpagefactory.H"
#include "x/w/shortcut.H"
#include "x/w/factory.H"
#include "x/w/label.H"
#include "x/w/bookpage_appearance.H"

LIBCXXW_NAMESPACE_START

bookpagefactoryObj::bookpagefactoryObj()
	: appearance{bookpage_appearance::base::theme()}
{
}

bookpagefactoryObj &bookpagefactoryObj
::set_appearance(const const_bookpage_appearance &new_appearance)
{
	appearance=new_appearance;
	return *this;
}

bookpagefactoryObj::~bookpagefactoryObj()=default;

void bookpagefactoryObj::do_add(const function<void (const factory &)> &tf,
				const function<void (const factory &)> &pf)
{
	do_add(tf, pf, {});
}

void bookpagefactoryObj::do_add(const text_param &label,
				const function<void (const factory &)> &f)
{
	do_add(label, f, {});
}

void bookpagefactoryObj::do_add(const text_param &label,
				const function<void (const factory &)> &f,
				const create_bookpage_with_label_args_t &args)
{
	std::optional<label_config> label_config_arg;
	std::optional<shortcut> shortcut_arg;

	const auto &label_config_value=
		optional_arg_or<label_config>(args, label_config_arg);
	const auto &shortcut_value=
		optional_arg_or<shortcut>(args, shortcut_arg);

	add([&]
	    (const factory &label_factory)
	    {
		    label_factory->create_label(label, label_config_value)
			    ->show();
	    },
	    [&]
	    (const factory &page_factory)
	    {
		    f(page_factory);
	    },
	    {shortcut_value});
}

LIBCXXW_NAMESPACE_END
