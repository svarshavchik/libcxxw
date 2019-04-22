/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "defaulttheme.H"
#include "theme_parser_lock.H"
#include "configfile.H"
#include "x/w/rgb.H"
#include "x/w/border_infomm.H"
#include "connection.H"
#include "connection_thread.H"
#include "screen.H"
#include "drawable.H"
#include "messages.H"
#include "x/w/impl/border_impl.H"
#include "picture.H"
#include "x/w/impl/background_color.H"
#include "x/w/uigenerators.H"
#include "uicompiler.H"
#include <x/property_value.H>
#include <x/chrcasecmp.H>
#include <x/strtok.H>
#include <x/join.H>
#include <x/glob.H>
#include <x/imbue.H>
#include <x/visitor.H>
#include <x/functionalrefptr.H>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <courier-unicode.h>

LIBCXXW_NAMESPACE_START

#ifndef THEMEDIR
#define THEMEDIR PKGDATADIR "/themes"
#endif

static property::value<std::string>
themedirbase(LIBCXX_NAMESPACE_STR "::w::themes", THEMEDIR);

static property::value<std::string>
themesubdirprop(LIBCXX_NAMESPACE_STR "::w::theme::name", "");

static property::value<int>
themescaleprop(LIBCXX_NAMESPACE_STR "::w::theme::scale", 0);

static property::value<std::string>
themeoptionsprop(LIBCXX_NAMESPACE_STR "::w::theme::options", "");

static const char use_primary_clipboard_option_id[]="_use_primary_clipboard";

std::string themedirroot()
{
	return themedirbase.get();
}

// Set a baseline value of how many pixels make up one millimeter, which
// must be at least one.

static inline dim_t one_millimeter(dim_t pixels, dim_t millimeters,
				   double scale)
{
	dim_t n=(dim_t::value_type)
		std::round(scale * (dim_t::value_type)pixels
			   / (dim_t::value_type)millimeters);

	if (n == 0)
		n=1;

	return n;
}

//////////////////////////////////////////////////////////////////////////////

// Miscellaneous parsing functions.

//////////////////////////////////////////////////////////////////////////////

// We have the theme property setting (maybe) and the configuration file
// (also maybe). Compute the theme name and scale.
//
// If the CXXWTHEME property is set in the root window, use that.
//
// If that's not set, check the configuration file.
//
// If all else fails, it's "default", 100% scale.

static std::string default_theme_name(const std::string &cxxwtheme_property,
				      const xml::doc &config,
				      const std::string &themenameArg)
{
	auto themename=themenameArg;

	if (themename.empty())
	{
		auto n=cxxwtheme_property.find(':');

		if (n != std::string::npos)
			return cxxwtheme_property.substr(n+1);

		auto lock=config->readlock();

		if (lock->get_root())
		{
			auto xpath=lock->get_xpath("/cxxw/theme/name");

			if (xpath->count())
			{
				xpath->to_node(1);
				themename=lock->get_text();
			}
		}
	}

	if (themename.empty() || access((themedirroot() + "/"
					+ themename + "/theme.xml").c_str(),
				       R_OK) < 0)
		themename="default";
	return themename;
}

static double default_theme_scale(const std::string &cxxwtheme_property,
				  const xml::doc &config,
				  int scale)
{
	if (scale == 0)
	{
		auto n=cxxwtheme_property.find(':');

		if (n != std::string::npos)
		{
			std::istringstream i(cxxwtheme_property.substr(0, n));

			i >> scale;

			if (!i.fail())
			{
				if (scale < SCALE_MIN || scale > SCALE_MAX)
					scale=100;

				return scale/100.0;
			}
		}

		auto lock=config->readlock();

		if (lock->get_root())
		{
			auto xpath=lock->get_xpath("/cxxw/theme/scale");

			if (xpath->count())
			{
				xpath->to_node(1);
				auto s=lock->get_text();

				std::istringstream(s) >> scale;
			}
		}
	}
	if (scale < SCALE_MIN || scale > SCALE_MAX)
		scale=100;

	return scale / 100.0;
}

static enabled_theme_options_t
default_theme_options(const std::string &cxxwtheme_property,
		      const xml::doc &config,
		      std::string themeoptions)
{
	enabled_theme_options_t enabled_options;

	std::istringstream i;

	if (themeoptions.empty())
	{
		auto n=cxxwtheme_property.find(':');

		if (n != std::string::npos)
		{
			i.str(cxxwtheme_property.substr(0,n));

			int ignore;

			i >> ignore; // Scaling factor.
		}
		else
		{
			auto lock=config->readlock();

			if (lock->get_root())
			{
				auto xp=lock->get_xpath("/cxxw/theme/option");

				int n=xp->count();

				for (int i=0; i<n; ++i)
				{
					xp->to_node(i+1);
					enabled_options
						.insert(lock->get_text());
				}
			}
			return enabled_options;
		}
	}
	else
	{
		i.str(themeoptions);
	}

	std::string word;

	while (i >> word)
		enabled_options.insert(word);

	return enabled_options;
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve CXXWTHEME property from screen 0's root window.

std::string defaulttheme::base
::cxxwtheme_property(const xcb_screen_t *screen_0,
		     const connection_thread &thread)
{
	std::string value;

	try {
		thread->info->collect_property_with
			(screen_0->root,
			 thread->info->atoms_info.cxxwtheme,
			 thread->info->atoms_info.string,
			 false,
			 [&]
			 (auto type, auto format, void *data, size_t size)
			 {
				 value.append(reinterpret_cast<char *>(data),
					      size);
			 });
	} catch (const exception &e)
	{
		auto ee=EXCEPTION(_("Error reading CXXWTHEME property: ")
				  << e);

		ee->caught();
	}

	return value;
}

// First cxxw app initializes CXXWTHEME property in screen 0's root from
// the configuration file.

void load_cxxwtheme_property(const xcb_screen_t *screen_0,
			     const connection_thread &thread,
			     const std::string &theme_name,
			     int theme_scale,
			     const enabled_theme_options_t
			     &enabled_theme_options)
{
	auto conn=thread->info;

	std::ostringstream o;

	o << theme_scale;

	for (const auto &opt:enabled_theme_options)
		o << " " << opt;

	o << ":" << theme_name;

	auto data=o.str();

	xcb_change_property(conn->conn,
			    XCB_PROP_MODE_REPLACE,
			    screen_0->root,
			    conn->atoms_info.cxxwtheme,
			    conn->atoms_info.string,
			    8,
			    data.size(),
			    data.c_str());
	xcb_flush(conn->conn);
}

void load_cxxwtheme_property(const screen &screen0,
			     const std::string &theme_name,
			     int theme_scale,
			     const enabled_theme_options_t
			     &enabled_theme_options)
{
	load_cxxwtheme_property(screen0->impl->xcb_screen,
				screen0->impl->thread,
				theme_name,
				theme_scale,
				enabled_theme_options);
}

std::tuple<std::string, int, enabled_theme_options_t>
connectionObj::current_theme() const
{
	auto theme=impl->screens.at(0)->current_theme.get();

	return {theme->themename, std::round(theme->themescale*100),
		theme->enabled_theme_options};
}

static std::vector<theme_option>
parse_available_theme_options(const xml::doc &theme_configfile)
{
	std::vector<theme_option> opts;

	theme_parser_lock lock{theme_configfile->readlock()};

	if (lock->get_root())
	{
		auto xpath=lock->get_xpath("/theme/option");

		size_t count=xpath->count();

		std::unordered_set<std::string> seen;

		for (size_t i=0; i<count; ++i)
		{
			xpath->to_node(i+1);

			auto descr_str=lock->get_text();

			auto id=lock->get_any_attribute("id");

			if (id.empty())
				throw EXCEPTION("Missing \"id\" "
						"attribute for an "
						"<option>");

			if (seen.find(id) != seen.end())
				continue;

			if (id.substr(0, 1) == "_")
				throw EXCEPTION("Theme option id cannot start "
						"with an underscore.");
			seen.insert(id);

			if (descr_str.empty())
				continue;
			auto descr=unicode::iconvert::tou
				::convert(descr_str, unicode::utf_8).first;

			opts.push_back({id, descr});
		}
	}

	static const struct {
		const char *id;
		const char32_t *description;
	} reserved_options[]={
			      {use_primary_clipboard_option_id,
			       U"Use the primary clipboard for Copy/Cut/Paste"}
	};

	for (const auto &option:reserved_options)
	{
		opts.push_back({option.id, option.description});
	}
	return opts;
}

static std::vector<theme_option>
make_available_theme_options(const defaultthemeObj::config &config)
{
	try {
		return parse_available_theme_options
			(config.theme_configfile);
	} catch (const exception &e)
	{
		throw EXCEPTION("An error occured while parsing the "
				<< config.themename
				<< " theme configuration file: "
				<< e);
	}
}

std::vector<connection::base::available_theme>
connection::base::available_themes()
{
	std::vector<connection::base::available_theme> themes;

	std::vector<std::string> filenames;

	glob::create()->expand(themedirroot() + "/*/theme.xml")->get(filenames);

	for (const auto &theme_xml:filenames)
	{
		try {
			// Extract <name> from each theme.

			auto xml=xml::doc::create(theme_xml,
						  "nonet xinclude");

			auto directory=
				theme_xml.substr(0, theme_xml.rfind('/'));

			std::string name=directory.substr(directory.rfind('/')+1);
			std::string description=name;

			auto options=parse_available_theme_options(xml);

			themes.push_back({name, description, options});
		} catch (const exception &e)
		{
			e->caught();
		}
	}

	// In alphabetical order.

	std::sort(themes.begin(), themes.end(),
		  [&]
		  (const auto &info1,
		   const auto &info2)
		  {
			  return info1.description < info2.description;
		  });

	return themes;
}

defaulttheme::base::config
defaulttheme::base::get_config(const std::string &property)
{
	auto user_configfile=read_config();
	auto themename=default_theme_name(property, user_configfile,
					  themesubdirprop.get());
	auto themescale=default_theme_scale(property, user_configfile,
					    themescaleprop.get());
	auto themeoptions=default_theme_options(property, user_configfile,
						themeoptionsprop.get());

	return get_config(themename, themescale, themeoptions);
}

defaulttheme::base::config
defaulttheme::base::get_config(const std::string &themename,
			       double themescale,
			       const enabled_theme_options_t
			       &enabled_theme_options)
{
	xml::docptr theme_configfile;

	auto themedir=themedirbase.get() + "/" + themename;

	auto filename=themedir + "/theme.xml";

	try {
		theme_configfile=xml::doc::create(filename, "nonet xinclude");
	} catch (const exception &e)
	{
		throw EXCEPTION("Error parsing " << filename
				<< ": " << e);
	}

	return { themename, themedir, themescale,
			enabled_theme_options,
			theme_configfile };
}

defaultthemeObj::defaultthemeObj(const xcb_screen_t *screen,
				 const config &theme_config)
	: themename{theme_config.themename},
	  themescale{theme_config.themescale},
	  available_theme_options{make_available_theme_options(theme_config)},
	  enabled_theme_options{theme_config.enabled_theme_options},
	  themedir{theme_config.themedir},
	  h1mm{one_millimeter(screen->width_in_pixels,
			      screen->width_in_millimeters, themescale)},
          v1mm{one_millimeter(screen->height_in_pixels,
			      screen->height_in_millimeters, themescale)},
	  screen{screen}
{
	try {
		theme_parser_lock
			lock{theme_config.theme_configfile->readlock()};

		{
			uicompiler compiler{lock, *this, false};
		}

		load_fonts(lock);
	} catch (const exception &e)
	{
		throw EXCEPTION("An error occured while parsing the "
				<< themename << " theme configuration file: "
				<< e);
	}
}

bool defaultthemeObj::is_different_theme(const const_defaulttheme &t) const
{
	if (themename != t->themename ||
	    themescale != t->themescale ||
	    enabled_theme_options != t->enabled_theme_options ||
	    available_theme_options.size() != t->available_theme_options.size())
		return true;

	auto oo=t->available_theme_options.begin();

	for (const auto &this_o:available_theme_options)
	{
		if (this_o.label != oo->label)
			return true;
		++oo;
	}
	return false;
}

defaultthemeObj::~defaultthemeObj()=default;

////////////////////////////////////////////////////////////////////////////
//
// Theme parsing functions.

// Take a dim_t, multiply it by a double scale, round off the result.

template<typename dim_type>
static dim_t dim_scale(dim_type &&orig, double scale)
{
	typedef typename std::remove_reference<dim_type>::type::value_type
		value_type;

	auto res=std::round(value_type(orig) * scale);

	if (res >= std::numeric_limits<dim_t::value_type>::max()-1)
		res=std::numeric_limits<dim_t::value_type>::max()-1;

	// However, if the scale is tiny, but is not zero,
	// we cannot scale it to 0.

	if (scale != 0 && res < 1)
		res=1;

	return (dim_t::value_type)res;
}

/////////////////////////////////////////////////////////////////////////////

dim_t defaultthemeObj::get_theme_dim_t(const dim_arg &id, themedimaxis wh)
	const
{
	auto v=std::visit(visitor{
				  [](double v)
				  {
					  return v;
				  },
				  [&](const std::string &id)
				  {
					  auto iter=dims.find(id);

					  if (iter == dims.end())
						  throw EXCEPTION
							  (gettextmsg
							   (_("Size %1% does"
							      " not exist"),
							    id));

					  return iter->second;
				  }
		}, id);

	return wh == themedimaxis::width
		? compute_width(v):compute_height(v);
}

dim_t defaultthemeObj::compute_width(double millimeters) const
{
	if (std::isnan(millimeters))
		return dim_t::infinite();

	if (millimeters < 0)
		return dim_t::truncate(-millimeters); // Pixels.

	auto scaled=std::round(themescale * millimeters *
			       screen->width_in_pixels /
			       screen->width_in_millimeters);

	// The calculated value may be large, but it won't be infinite.
	if (scaled > dim_t::infinite()-1)
		scaled=dim_t::infinite()-1;

	// If the number of millimeters is not 0, but small, make sure we'll
	// calculated at least 1 pixel. The only way the calculated pixel
	// count is 0 would be if millimeters was 0.
	if (millimeters != 0 && scaled < 1)
		scaled=1;

	return dim_t::value_type(scaled);
}

dim_t defaultthemeObj::compute_height(double millimeters) const
{
	if (std::isnan(millimeters))
		return dim_t::infinite();

	if (millimeters < 0)
		millimeters= -millimeters;

	auto scaled=std::round(themescale * millimeters *
			       screen->height_in_pixels /
			       screen->height_in_millimeters);

	// The calculated value may be large, but it won't be infinite.
	if (scaled > dim_t::infinite()-1)
		scaled=dim_t::infinite()-1;

	// If the number of millimeters is not 0, but small, make sure we'll
	// calculated at least 1 pixel. The only way the calculated pixel
	// count is 0 would be if millimeters was 0.
	if (millimeters != 0 && scaled < 1)
		scaled=1;

	return dim_t::value_type(scaled);
}

theme_color_t defaultthemeObj::get_theme_color(const std::string_view &id) const
{
	std::vector<std::string> ids;

	if (!id.empty())
		strtok_str(id, ", \r\t\n", ids);

	for (const auto &try_id:ids)
	{
		for (const auto &option:available_theme_options)
		{
			if (enabled_theme_options.find(option.label) ==
			    enabled_theme_options.end())
				continue;
			auto iter=colors.find(option.label + ":"
					      + try_id);

			if (iter != colors.end())
				return iter->second;
		}

		auto iter=colors.find(try_id);

		if (iter != colors.end())
			return iter->second;
	}

	throw EXCEPTION(gettextmsg(_("Theme color %1% does not exist"), id));
}


const border_infomm &
defaultthemeObj::get_theme_border(const std::string &id) const
{
	auto iter=borders.find(id);

	if (iter != borders.end())
		return iter->second;

	throw EXCEPTION(gettextmsg(_("Theme border %1% does not exist"),
				   id));
}

//////////////////////////////////////////////////////////////////////////////

void defaultthemeObj::load_fonts(const theme_parser_lock &root_lock)
{
	auto lock=root_lock.clone();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/font");

	load_fonts(lock, xpath,
		   [&, this]
		   (const std::string &id, const auto &new_font)
		   {
			   fonts.insert({id, new_font});
		   },
		   [this]
		   (const std::string &from,
		    auto &new_font)
		   {
			   return false;
		   });
}

void defaultthemeObj::do_load_fonts(const theme_parser_lock &lock,
				    const xml::doc::base::xpath &xpath,
				    const function<void (const std::string &,
							 const font &)>
				    &install,
				    const function<bool (const std::string &,
							 font &)> &lookup)
	const
{
	size_t count=xpath->count();

	bool parsed;

	// Repeatedly pass over all fonts, parsing the ones that are not based
	// on unparsed fonts.

	do
	{
		parsed=false;

		for (size_t i=0; i<count; ++i)
		{
			xpath->to_node(i+1);

			auto id=lock->get_any_attribute("id");

			if (id.empty())
				throw EXCEPTION(_("no id specified for font"));

			font new_font;

			if (fonts.find(id) != fonts.end() ||
			    lookup(id, new_font))
				continue; // Did this one already.

			auto from=lock->get_any_attribute("from");

			if (!from.empty())
			{
				auto iter=fonts.find(from);

				if (iter != fonts.end())
					new_font=iter->second;
				else
					if (!lookup(from, new_font))
						// Not yet parsed
						continue;
			}

			static const struct {
				const char *name;
				font &(font::*handler)(double);
			} double_values[]={
				{ "point_size", &font::set_point_size},
				{ "scaled_size", &font::set_scaled_size},
				{ "scale", &font::scale},
			};

			for (const auto &v:double_values)
			{
				double value;
				auto node=lock.clone();

				auto xpath=node->get_xpath(v.name);

				if (xpath->count() == 0)
					continue;

				xpath->to_node();

				std::istringstream i(node->get_text());

				imbue<std::istringstream> imbue{lock.c_locale,
						i};

				i >> value;

				if (i.fail())
					throw EXCEPTION(gettextmsg(_("Cannot parse %1%, font id=%2%"),
								   v.name,
								   id));
				(new_font.*(v.handler))(value);
			}

			static const struct {
				const char *name;
				font &(font::*handler1)(const std::string &);
				font &(font::*handler2)(const std::string_view &);
			} string_values[]={
				{ "family", &font::set_family, nullptr},
				{ "foundry", &font::set_foundry, nullptr},
				{ "style", &font::set_style, nullptr},
				{ "weight", nullptr, &font::set_weight},
				{ "spacing", nullptr, &font::set_spacing},
				{ "slant", nullptr, &font::set_slant},
				{ "width", nullptr, &font::set_width},
			};

			for (const auto &v:string_values)
			{
				auto node=lock.clone();

				auto xpath=node->get_xpath(v.name);

				if (xpath->count() == 0)
					continue;

				xpath->to_node();

				if (v.handler1)
					(new_font.*(v.handler1))
						(node->get_text());
				if (v.handler2)
					(new_font.*(v.handler2))
						(node->get_text());
			}
			install(id, new_font);
			parsed=true;
		}
	} while (parsed);

	font new_font;

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (fonts.find(id) == fonts.end() &&
		    !lookup(id, new_font))
			throw EXCEPTION(gettextmsg(_("circular or non-existent dependency of font id=%1%"),
						   id));
	}
}

font defaultthemeObj::get_theme_font(const std::string &id) const
{
	auto semicolon=id.find(';');

	if (semicolon != id.npos)
	{
		auto base_font=get_theme_font(id.substr(0, semicolon));

		base_font += id.substr(++semicolon);

		return base_font;
	}

	auto iter=fonts.find(id);

	if (iter != fonts.end())
		return iter->second;

	throw EXCEPTION(gettextmsg(_("Theme font %1% does not exist"),
				   id));
}

//////////////////////////////////////////////////////////////////////////////


std::string single_value(const theme_parser_lock &lock,
			 const char *element,
			 const char *parent)
{
	auto v=lock.clone();

	auto xpath=v->get_xpath(element);

	if (xpath->count() != 1)
		throw EXCEPTION(gettextmsg(_("<%1%> must contain exactly one <%2%>"),
					   parent, element));

	xpath->to_node(1);
	return v->get_text();
}

std::string optional_value(const theme_parser_lock &lock,
			   const char *element,
			   const char *parent)
{
	auto v=lock.clone();

	auto xpath=v->get_xpath(element);

	size_t n=xpath->count();

	if (n > 1)
		throw EXCEPTION(gettextmsg(_("<%1%> cannot have more than one <%2%>"),
					   parent, element));

	if (!n) return {};

	xpath->to_node(1);
	return v->get_text();
}

std::string lowercase_single_value(const theme_parser_lock &lock,
				   const char *element,
				   const char *xpath)
{
	std::string s=single_value(lock, element, xpath);

	std::transform(s.begin(), s.end(), s.begin(), chrcasecmp::tolower);

	return s;
}

bool single_value_exists(const theme_parser_lock &lock,
			 const char *element)
{
	auto v=lock.clone();

	auto xpath=v->get_xpath(element);

	return xpath->count() > 0;
}

template<typename to_value_t>
static to_value_t to_value(const theme_parser_lock &lock,
			   const std::string &value,
			   const char *element)
{
	to_value_t v;

	std::istringstream i(value);

	imbue<std::istringstream> imbue{lock.c_locale, i};

	i >> v;

	if (i.fail())
		throw EXCEPTION(gettextmsg(_("Cannot convert the value of <%1%>"
					     ), element));
	return v;
}

dim_t to_dim_t(const theme_parser_lock &lock,
	       const char *element, const char *parent)
{
	return to_value<dim_t>(lock,
			       single_value(lock, element, parent), element);
}

size_t to_size_t(const theme_parser_lock &lock,
		 const char *element, const char *parent)
{
	return to_value<size_t>(lock,
				single_value(lock, element, parent), element);
}

int to_percentage_t(const theme_parser_lock &lock,
		    const char *element, const char *parent)
{
	int v=to_value<int>(lock,
			    single_value(lock, element, parent), element);

	if (v < 0 || v > 100)
		throw EXCEPTION(gettextmsg(_("Value of <%1%> is not 0-100"
					     ), element));

	return v;
}

halign to_halign_value(const theme_parser_lock &lock,
		       const char *element, const char *parent)
{
	auto value=single_value(lock, element, parent);

	std::transform(value.begin(), value.end(), value.begin(),
		       chrcasecmp::tolower);

	halign v;

	if (value == "left")
	{
		v=halign::left;
	}
	else if (value == "centered")
	{
		v=halign::center;
	}
	else if (value == "right")
	{
		v=halign::right;
	}
	else if (value == "fill")
	{
		v=halign::fill;
	}
	else
		throw EXCEPTION(gettextmsg(_("\"%1%\" is not a valid setting for <%2%>"),
					   value, element));

	return v;
}

valign to_valign_value(const theme_parser_lock &lock,
		       const char *element, const char *parent)
{
	auto value=single_value(lock, element, parent);

	std::transform(value.begin(), value.end(), value.begin(),
		       chrcasecmp::tolower);

	valign v;

	if (value == "top")
	{
		v=valign::top;
	}
	else if (value == "middle")
	{
		v=valign::middle;
	}
	else if (value == "bottom")
	{
		v=valign::bottom;
	}
	else if (value == "fill")
	{
		v=valign::fill;
	}
	else
		throw EXCEPTION(gettextmsg(_("\"%1%\" is not a valid setting for <%2%>"),
					   value, element));

	return v;
}

const char *defaultthemeObj::default_cut_paste_selection() const
{
	return enabled_theme_options.find(use_primary_clipboard_option_id) ==
		enabled_theme_options.end()
		? "SECONDARY":"PRIMARY";
}


LIBCXXW_NAMESPACE_END
