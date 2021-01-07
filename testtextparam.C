/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/exception.H>
#include <x/options.H>
#include "x/w/impl/richtext/richtextstring.H"
#include "x/w/impl/richtext/richtextmeta.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/theme_text.H"
#include "x/w/uigenerators.H"
#include "x/w/shortcut.H"
#include "screen.H"
#include "main_window.H"
#include "main_window_handler.H"
#include "x/w/impl/background_color.H"
#include <X11/keysym.h>

#include <x/messages.H>
#include <x/locale.H>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

void testtextparam(const main_window &mw)
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
		mw->impl->handler
		->create_richtextstring({create_new_background_color
				(mw->get_screen(),
				 mw->elementObj::impl
				 ->get_window_handler()
				 .drawable_pictformat,
				 "0%"),
				mw->impl->handler
				->create_current_fontcollection
				(theme_font{"serif"})},
			param2);

	const auto &m=string.get_meta();
	const auto &s=string.get_string();

	if (m.size() != 2 ||
	    m.at(0).first != 0 ||
	    m.at(1).first != 3 ||
	    s.size() != 10 ||
	    m.at(0).second.bg_color.null() ||
	    !m.at(1).second.bg_color.null())
		throw EXCEPTION("Sumthin's wrong.");
}

void testrgb()
{
	std::string s=maroon;

	std::cout << s << std::endl;
	rgb color{s};

	if (color != maroon)
	{
		throw EXCEPTION("wrong.");
	}

	color=rgb{"b=80"};

	if (color != rgb{"r=0000, g=0000, b=8080, a=ffff"})
	{
		throw EXCEPTION("wrong.");
	}
}

void testthemetext()
{
	text_param t{ theme_text{"${font:some_theme_font}${color:white}"
				 "${decoration:underline}"
				 "O$$${decoration:none}k"}};


	if (t.string != U"O$k" ||
	    t.colors !=
	    std::unordered_map<size_t, text_color_arg>
	    {{0, text_color_arg{rgb{"r=ffff, g=ffff, b=ffff, a=ffff"}}}} ||
	    t.fonts != std::unordered_map<size_t, font_arg>
	    {{0, theme_font{"some_theme_font"}}} ||
	    t.decorations != std::unordered_map<size_t, text_decoration>{
		    {0, text_decoration::underline},
		    {2, text_decoration::none},
	    })
		throw EXCEPTION("theme_text parsing failed");
}

void testthemetext2()
{
	text_param t{ theme_text{"${context:label}message-label",
				 uigenerators::create
				 ("./testtextparam_dummy_xml",
				  messages::create("xtest", "xtest",
						   locale::create("en_US.UTF8"))
				  )}};

	if (t.string != U"translated-label")
		throw EXCEPTION("Localized label failed");
}

void testthemetext3()
{
	text_param t{ theme_text{"Lorem\t     $#   \n     $#   \nIpsum"}};

	if (t.string != U"Lorem Ipsum")
		throw EXCEPTION("Comment failed");

	text_param t2{ theme_text{"      $#\nLorem Ipsum\t     $#   \n\nDolor"}};

	if (t2.string != U"Lorem Ipsum \nDolor")
		throw EXCEPTION("Comment failed");
}

void testshortcut()
{
	shortcut w{'W'};

	if (w.unicode != 'W')
		throw EXCEPTION("char32_t shortcut constructor failed");

	shortcut alth{"Alt", 'H'};

	if (alth.description() != "Alt-H")
		throw EXCEPTION("2 arg shortcut constructor failed");

	shortcut f1{"F1"};

	if (f1.keysym != XK_F1)
		throw EXCEPTION("F1 shortcut constructor failed");

	shortcut altf2{"${context}Alt-F2"};

	if (altf2.description() != "Alt-F2")
		throw EXCEPTION("shortcut with label constructor failed");

	shortcut kp_home{"KP_Home"};

	if (kp_home.label() != U"KP-Home")
		throw EXCEPTION("shortcut.label() failed");
}

int main(int argc, char **argv)
{
	try {
		locale::base::environment()->global();

		auto options=option::list::create();

		options->addDefaultOptions();

		auto parser=option::parser::create();

		parser->setOptions(options);

		int flag=parser->parseArgv(argc, argv);

		if (flag == 0)
			flag=parser->validate();

		if (flag == option::parser::base::err_builtin)
			exit(0);

		auto mw=main_window::create([&]
					    (const auto &ignore)
					    {
					    });

		testtextparam(mw);
		testrgb();
		testthemetext();
		testthemetext2();
		testthemetext3();
		testshortcut();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
