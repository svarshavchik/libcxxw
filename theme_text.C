/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/text_param.H"
#include "x/w/theme_text.H"
#include "x/w/uigenerators.H"
#include "uicompiler.H"
#include "messages.H"
#include <algorithm>

LIBCXXW_NAMESPACE_START

static const_uigenerators &cached_default_generator()
{
	static const_uigenerators config=
		ptrref_base::objfactory<const_uigenerators>::create();

	return config;
}

theme_text::theme_text(const std::string &s)
	: theme_text{s, cached_default_generator()}
{
}

theme_text::theme_text(const std::string &text,
		       const const_uigenerators &generators)
	: text{text}, generators{generators}
{
}

theme_text::~theme_text()=default;

theme_text::theme_text(const theme_text &)=default;
theme_text::theme_text(theme_text &&)=default;
theme_text &theme_text::operator=(const theme_text &)=default;
theme_text &theme_text::operator=(theme_text &&)=default;

text_param &text_param::operator()(const theme_text &text)
{
	auto b=text.text.data(), e=b+text.text.size();

	while (b != e)
	{
		auto p=std::find(b, e, '$');

		if (p != b)
		{
			operator()(std::string_view{b, (size_t)(p-b)});
			b=p;
			continue;
		}

		if ((b=++p) == e)
			continue;

		if (*b == '$')
		{
			++b;
			operator()("$");
			continue;
		}

		if (*(p=b) != '{' || (b=std::find(++p, e, '}')) == e)
			throw EXCEPTION(_("Cannot parse theme_text macro"));

		std::string_view macro{p, (size_t)(b-p)};

		++b;

		auto mb=macro.data();
		auto me=mb+macro.size();

		p=std::find(mb, me, ':');

		if (p != me)
			++p;

		std::string_view name{mb, (size_t)(p-mb)};

		// TODO: std::string should not be necessary in C++20
		std::string value{p, (size_t)(me-p)};

		if (name == "color:")
		{
			operator()(uicompiler::to_text_color_arg
				   (text.generators->lookup_color(value, true,
								  "${color}"),
				    "${color}", "text_param"));
			continue;
		}
		else if (name == "font:")
		{
			operator()(text.generators->lookup_font(value, true,
								"${font}"));
			continue;
		}
		else if (name == "decoration:")
		{
			if (value == "underline")
			{
				operator()(text_decoration::underline);
				continue;
			}
			else if (value == "none")
			{
				operator()(text_decoration::none);
				continue;
			}
		}
		throw EXCEPTION(gettextmsg(_("Cannot parse \"${%1%}\" "
					     "theme_text macro"), macro));
	}

	return *this;
}

LIBCXXW_NAMESPACE_END
