/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/uigenerators.H"
#include "x/w/all_appearances.H"
#include "uicompiler.H"
#include "messages.H"
#include "theme_parser_lock.H"
#include <x/xml/doc.H>
#include <functional>

LIBCXXW_NAMESPACE_START

const_uigenerators uigeneratorsBase::create(const std::string_view &filename)
{
	auto xml=xml::doc::create(filename, "nonet xinclude");

	auto g=ptrref_base::objfactory<uigenerators>::create();

	theme_parser_lock lock{xml->readlock()};

	if (lock->get_root())
	{
		uicompiler compiler{lock, g, true};
	}

	return g;
}

uigeneratorsObj::uigeneratorsObj()
	: 	// Install default HTML 3.2 colors

	colors{
	       {"transparent", transparent},
	       {"black", black},
	       {"gray", gray},
	       {"silver", silver},
	       {"white", white},
	       {"maroon", maroon},
	       {"red", red},
	       {"olive", olive},
	       {"yellow", yellow},
	       {"green", green},
	       {"lime", lime},
	       {"teal", teal},
	       {"aqua", aqua},
	       {"navy", navy},
	       {"blue", blue},
	       {"fuchsia", fuchsia},
	       {"purple", purple}}
{
}


uigeneratorsObj::~uigeneratorsObj()=default;

dim_arg uigeneratorsObj::lookup_dim(const std::string &name,
				   bool allowthemerefs,
				   const char *tag) const
{
	auto iter=dims.find(name);

	if (iter != dims.end())
		return iter->second;

	if (!allowthemerefs)
		throw EXCEPTION(gettextmsg
				(_("The %1% <dim> was not found for %2%"),
				 name, tag));

	return name;
}

border_arg uigeneratorsObj::lookup_border(const std::string &name,
					  bool allowthemerefs,
					  const char *tag) const
{
	auto iter=borders.find(name);

	if (iter != borders.end())
		return iter->second;

	if (!allowthemerefs)
		throw EXCEPTION(gettextmsg
				(_("The %1% <border> was not found for %2%"),
				 name, tag));
	return name;
}

color_arg uigeneratorsObj::lookup_color(const std::string &name,
					bool allowthemerefs,
					const char *tag) const
{
	auto iter=colors.find(name);

	if (iter != colors.end())
		return std::visit([]
				  (const auto &c) -> color_arg
				  {
					  return c;
				  }, iter->second);

	if (!allowthemerefs)
		throw EXCEPTION(gettextmsg
				(_("The %1% <color> was not found for %2%"),
				 name, tag));

	return name;
}

font_arg uigeneratorsObj::lookup_font(const std::string &name,
				      bool allowthemerefs,
				      const char *tag) const
{
	auto semicolon=name.find(';');

	if (semicolon != name.npos)
	{
		auto base_font_arg=lookup_font(name.substr(0, semicolon),
					       false, tag);

		auto &base_font=std::get<font>(base_font_arg);
		base_font += name.substr(++semicolon);

		return base_font_arg;
	}

	auto iter=fonts.find(name);

	if (iter != fonts.end())
		return iter->second;

	if (allowthemerefs)
		return theme_font{name};

	throw EXCEPTION(gettextmsg(_("%2%: font %1% does not exist"),
				   name, tag));
}

// Instantiate all implemented do_lookup_appearance()s.

const_appearance
uigeneratorsObj::lookup_appearance(const std::string_view &name) const
{
	//! TODO: should be std::string_view in C++20
	auto iter=loaded_appearances.find(std::string{name.begin(),
							      name.end()});

	if (iter == loaded_appearances.end())
		throw EXCEPTION(gettextmsg(_("appearance not found: %1%"
					     " (could also be a circular "
					     "reference)"
					     ),
					   name));

	return iter->second;
}

LIBCXXW_NAMESPACE_END
