/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/exception.H>
#include <x/options.H>
#include "richtext/richtextstring.H"
#include "richtext/richtextmeta.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "screen.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "x/w/impl/background_color.H"

using namespace LIBCXX_NAMESPACE::w;

void testtextparam(const auto &main_window)
{
	text_param param1{"foo", U"bar", rgb(0,0,0),
			theme_font{"sans_serif"},
			"0%"_color, "bar"};

	text_param param2(rgb(0,0,0), rgb(0,0,0),
			  "foo",
			  "sans_serif"_theme_font,
			  "0%"_color,
			  "foobaz");

	auto string=
		main_window->impl->handler
		->create_richtextstring( {main_window->get_screen()
					->impl
					->create_background_color("0%"),
					main_window->impl->handler
					->create_theme_font("serif")},
			param2);

	const auto &m=string.get_meta();
	const auto &s=string.get_string();

	if (m.size() != 2 ||
	    m.at(0).first != 0 ||
	    m.at(1).first != 3 ||
	    s.size() != 9 ||
	    m.at(0).second.bg_color.null() ||
	    !m.at(1).second.bg_color.null())
		throw EXCEPTION("Sumthin's wrong.");
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::w::themes",
					"themes", true, true);

		LIBCXX_NAMESPACE::locale::base::environment()->global();

		auto options=LIBCXX_NAMESPACE::option::list::create();

		options->addDefaultOptions();

		auto parser=LIBCXX_NAMESPACE::option::parser::create();

		parser->setOptions(options);

		int flag=parser->parseArgv(argc, argv);

		if (flag == 0)
			flag=parser->validate();

		if (flag == LIBCXX_NAMESPACE::option::parser::base::err_builtin)
			exit(0);

		auto mw=main_window::create([&]
					    (const auto &ignore)
					    {
					    });

		testtextparam(mw);

	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
