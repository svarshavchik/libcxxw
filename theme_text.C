/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/text_param.H"
#include "x/w/theme_text.H"
#include "x/w/uigenerators.H"
#include <x/messages.H>
#include <x/singleton.H>
#include "uicompiler.H"
#include "messages.H"
#include <algorithm>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

class cached_default_generatorObj : virtual public obj {

public:

	const_uigenerators config=
		ptrref_base::objfactory<const_uigenerators>::create();
};

#if 0
{
#endif
}

theme_text::theme_text(const std::string &s)
	: theme_text{s, singleton<cached_default_generatorObj>::get()->config}
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

text_param &text_param::operator()(explicit_theme_text text)
{
	std::u32string utext;

	if (text.t.generators->catalog)
	{
		utext=text.t.generators->catalog->get_unicode(text.t.text);
	}
	else
	{
		unicode::iconvert::tou::convert(text.t.text.begin(),
						text.t.text.end(),
						unicode_locale_chset(),
						utext);
	}

	auto b=utext.begin();
	auto e=utext.end();

	while (b != e)
	{
		auto p=std::find(b, e, '$');

		if (p != b)
		{
			size_t n=p-b;
			auto s=&*b;
			b=p;

			bool squashed=false;

			if (e-b > 1 && b[1] == '#')
			{
				// Squash spaces before the comment.

				while (n > 0 && (s[n-1] == ' '||
						 s[n-1] == '\t'))
				{
					squashed=true;
					--n;
				}

			}

			if (n)
			{
				operator()(std::u32string_view{s, n});
				if (squashed)
					operator()(U" ");
			}
			continue;
		}

		if ((b=++p) == e)
			continue;

		if (*b == '$')
		{
			++b;
			operator()(U"$");
			continue;
		}

		if (*b == '#')
		{
			while (b != e)
			{
				if (*b++ == '\n')
					break;
			}
			continue;
		}
		if (*(p=b) != '{' || (b=std::find(++p, e, '}')) == e
		    || std::find_if(p, b,
				    [](auto c)
				    {
					    return c >= 0x80;
				    }) != b)
			throw EXCEPTION(_("Cannot parse theme_text macro"));

		auto mb=p;
		auto me=b;
		++b;

		p=std::find(mb, me, ':');

		if (p != me)
			++p;

		std::u32string_view name{&*mb, (size_t)(p-mb)};

		std::string value{p, me};

		if (name == U"color:")
		{
			operator()(uicompiler::to_text_color_arg
				   (text.t.generators->lookup_color(value, true,
								    "${color}"),
				    "${color}", "text_param"));
			continue;
		}
		else if (name == U"font:")
		{
			operator()(text.t.generators->lookup_font(value, true,
								  "${font}"));
			continue;
		}
		else if (name == U"decoration:")
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
		else if (name == U"context:")
			continue;

		throw EXCEPTION(gettextmsg(_("Cannot parse \"${%1%}\" "
					     "theme_text macro"),
					   std::string{name.begin(),
							       name.end()}));
	}

	return *this;
}

std::u32string theme_text::get_unicode() const
{
	return text_param{*this}.string;
}

LIBCXXW_NAMESPACE_END
