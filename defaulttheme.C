/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "defaulttheme.H"
#include "configfile.H"
#include "x/w/rgb.H"
#include "connection.H"
#include "connection_thread.H"
#include "screen.H"
#include "drawable.H"
#include "messages.H"

#include <x/property_value.H>
#include <x/chrcasecmp.H>
#include <x/strtok.H>
#include <x/join.H>
#include <x/glob.H>
#include <x/xml/doc.H>

#include <sstream>
#include <cmath>
#include <algorithm>

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

std::string themedirroot()
{
	return themedirbase.getValue();
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

#if 0
// Look up something optional.

static bool if_given(const xml::doc::base::readlock &lock,
		     const char *xpath_node)
{
	auto xpath=lock->get_xpath(xpath_node);

	return xpath->count() > 0;
}

template<typename value>
static bool update_if_given(const xml::doc::base::readlock &lock,
			    const char *xpath_node,
			    value &v,
			    const char *descr,
			    const std::string &id)
{
	auto node=lock->clone();

	auto xpath=node->get_xpath(xpath_node);

	if (xpath->count() == 0)
		return false;

	xpath->to_node();

	std::istringstream i(node->get_text());

	i >> v;

	if (i.fail())
		throw EXCEPTION(gettextmsg(_("Cannot parse %1%, id=%2%"),
					   descr, id));
	return true;
}

template<typename value, typename functor>
static bool update_and_validate_if_given(const xml::doc::base::readlock &lock,
					 const char *xpath_node,
					 value &v,
					 const char *descr,
					 const std::string &id,
					 functor &&f)
{
	if (!update_if_given(lock, xpath_node, v, descr, id))
		return false;

	if (!f(v))
		throw EXCEPTION(gettextmsg(_("Invalid value for %1%, id=%2%"),
					   descr, id));
	return true;
}
#endif

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
				      const x::xml::doc &config,
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
				  const x::xml::doc &config,
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

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve CXXWTHEME property from screen 0's root window.

static std::string cxxwtheme_property(const xcb_screen_t *screen_0,
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
		std::cerr << _("Error reading CXXWTHEME property: ")
			  << e << std::endl;
	}

	return value;
}

// First cxxw app initializes CXXWTHEME property in screen 0's root from
// the configuration file.

static void load_cxxwtheme_property(const xcb_screen_t *screen_0,
				    const connection_thread &thread,
				    const std::string &theme_name,
				    int theme_scale)
{
	auto conn=thread->info;

	std::ostringstream o;

	o << theme_scale << ":" << theme_name;

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
			     int theme_scale)
{
	load_cxxwtheme_property(screen0->impl->xcb_screen,
				screen0->impl->thread,
				theme_name,
				theme_scale);
}

// Used by cxxwtheme tool, which overrides the theme to the default, for
// itself, to find the currently set theme.

std::pair<std::string, int> connectionObj::current_theme() const
{
	auto property=cxxwtheme_property(impl->screens.at(0)->xcb_screen,
					 impl->thread);
	auto config=read_config(); // Configuration file

	return {
		default_theme_name(property, config, ""),
			(int)(default_theme_scale(property, config, 0)
			      *100)
			};
}

void connectionObj::set_theme(const std::string &identifier,
			      int factor)
{
	auto available_themes=connection::base::available_themes();

	if (std::find_if(available_themes.begin(),
			 available_themes.end(),
			 [&]
			 (const auto &t)
			 {
				 return t.identifier == identifier;
			 }) == available_themes.end())
		throw EXCEPTION(gettextmsg(_("No such theme: %1%"),
					   identifier));

	if (factor < SCALE_MIN || factor > SCALE_MAX)
		throw EXCEPTION(gettextmsg(_("Theme scaling factor must be between %1% and %2%"),
					   SCALE_MIN, SCALE_MAX));

	load_cxxwtheme_property(impl->screens.at(0)->xcb_screen,
				impl->thread,
				identifier,
				factor);
}

std::vector<connection::base::available_theme>
connection::base::available_themes()
{
	std::vector<connection::base::available_theme> themes;

	std::vector<std::string> filenames;

	glob::create()->expand(themedirroot() + "/*/theme.xml")->get(filenames);

	for (const auto &theme_xml:filenames)
	{
		// Extract <name> from each theme.

		auto xml=x::xml::doc::create(theme_xml,
					     "nonet xinclude");

		auto directory=
			theme_xml.substr(0, theme_xml.rfind('/'));

		std::string name=directory.substr(directory.rfind('/')+1);
		std::string description=name;

		auto root=xml->readlock();

		if (root->get_root())
		{
			auto xpath=root->get_xpath("/theme/name");

			if (xpath->count())
			{
				xpath->to_node(1);
				description=root->get_text();
			}
		}

		themes.push_back({name, description});
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
defaulttheme::base::get_config(const xcb_screen_t *screen_0,
			       const connection_thread &thread)
{
	auto property=cxxwtheme_property(screen_0, thread);
	auto user_configfile=read_config();
	auto themename=default_theme_name(property, user_configfile,
					  themesubdirprop.getValue());
	auto themescale=default_theme_scale(property, user_configfile,
					    themescaleprop.getValue());

	xml::docptr theme_configfile;


	auto filename=themedirbase.getValue() + "/" + themename + "/theme.xml";

	try {
		theme_configfile=xml::doc::create(filename, "nonet xinclude");
	} catch (const exception &e)
	{
		std::cerr << "Error parsing " << filename
			  << ": " << e << std::endl;
	}

	return { screen_0, thread, property, themename, themescale,
			theme_configfile };
}

defaultthemeObj::defaultthemeObj(const xcb_screen_t *screen,
				 const config &theme_config)
	: themename(theme_config.themename),
	  themescale(theme_config.themescale),
	  h1mm(one_millimeter(screen->width_in_pixels,
			      screen->width_in_millimeters, themescale)),
	  v1mm(one_millimeter(screen->height_in_pixels,
			      screen->height_in_millimeters, themescale))
{
	// If we did not find the CXXWTHEME property, now is the time to
	// set it.

	if (theme_config.cxxwtheme_property.empty())
		load_cxxwtheme_property(theme_config.screen_0,
					theme_config.thread,
					theme_config.themename,
					theme_config.themescale * 100);

	try {
		if (!theme_config.theme_configfile.null())
		{
			xml::doc config=theme_config.theme_configfile;

			load_dims(config);
			load_colors(config);
			load_color_gradients(config);
		}
	} catch (const exception &e)
	{
		std::cerr << "An error occured while parsing the "
			  << themename << " theme configuration file: "
			  << e << std::endl;
		_exit(1);
	}
}

defaultthemeObj::~defaultthemeObj()=default;

template<typename dim_type>
static dim_t dim_scale(dim_type &&orig,
		       double scale)
{
	typedef typename std::remove_reference<dim_type>::type::value_type
		value_type;

	auto res=std::round(value_type(orig) * scale);

	if (res >= std::numeric_limits<dim_t::value_type>::max())
		res=std::numeric_limits<dim_t::value_type>::max()-1;

	return (dim_t::value_type)res;
}

// Parse the dims in the config file

static bool parse_dim(const xml::doc::base::readlock &lock,
		      const std::unordered_map<std::string, dim_t>
		      &existing_dims,
		      dim_t h1mm, dim_t v1mm, dim_t &mm,
		      const char *descr,
		      const std::string &id)
{
	auto scale=lock->get_any_attribute("scale");

	double v;

	{
		std::istringstream i(lock->get_text());

		i >> v;

		if (i.fail())
			throw EXCEPTION(gettextmsg(_("could not parse %1% id=%2%"),
						   descr,
						   id));
	}

	if (!scale.empty())
	{
		auto iter=existing_dims.find(scale);

		if (iter == existing_dims.end())
			return false; // Not yet.

		mm=dim_scale(iter->second, v);
	}
	else
	{
		auto axis=lock->get_any_attribute("axis");

		switch (chrcasecmp::tolower(*axis.c_str())) {
		case 'h':
			mm=dim_scale(h1mm, v);
			break;
		case 'v':
			mm=dim_scale(v1mm, v);
			break;
		case 'p':
			mm=(dim_t::value_type)v;
			break;
		default:
			mm=dim_scale(h1mm+v1mm, v/2);
			break;
		}
	}

	dim_t min_value=0;

	auto min_value_s=lock->get_any_attribute("min");

	if (!min_value_s.empty())
	{
		std::istringstream(min_value_s) >> min_value;
	}

	if (min_value > mm)
		mm=min_value;

	return true;
}

static void unknown_dim(const char *element, const std::string &id)
	__attribute__((noreturn));

static void unknown_dim(const char *element, const std::string &id)
{
	throw EXCEPTION(gettextmsg(_("circular or non-existent dependency of dim %1%=%2%"),
				   element,
				   id));
}

#if 0
// Look up a dimension, when parsing something else.

static void update_dim_if_given(const xml::doc::base::readlock &lock,
				const char *xpath_node,
				const std::unordered_map<std::string, dim_t
				> &existing_dims,
				dim_t h1mm, dim_t v1mm, dim_t &mm,
				const char *descr,
				const std::string &id)
{
	auto node=lock->clone();

	auto xpath=node->get_xpath(xpath_node);

	if (xpath->count() == 0)
		return;

	xpath->to_node();

	if (!parse_dim(node, existing_dims, h1mm, v1mm, mm, descr, id))
		unknown_dim(descr, id);
}
#endif

void defaultthemeObj::load_dims(const xml::doc &config)
{
	auto lock=config->readlock();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/dim");

	size_t count=xpath->count();

	bool parsed;

	// Repeatedly pass over all dims, parsing the ones that are not based
	// on unparsed dims.
	do
	{
		parsed=false;

		for (size_t i=0; i<count; ++i)
		{
			xpath->to_node(i+1);

			auto id=lock->get_any_attribute("id");

			if (id.empty())
				throw EXCEPTION(_("no id specified for dim"));

			if (dims.find(id) != dims.end())
				continue; // Did this one already.

			dim_t mm;

			if (!parse_dim(lock, dims, h1mm, v1mm, mm, "dim", id))
				continue;
			dims.insert(std::make_pair(id, mm));
			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (dims.find(id) == dims.end())
			unknown_dim("dim", id);
	}
}

///////////////////////////////////////////////////////////////////////////////

// Parse colors

void defaultthemeObj::load_colors(const xml::doc &config)
{
	auto lock=config->readlock();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/color");

	size_t count=xpath->count();

	bool parsed;

	// Repeatedly pass over all colors, parsing the ones that are not based
	// on unparsed colors.

	do
	{
		parsed=false;

		for (size_t i=0; i<count; ++i)
		{
			xpath->to_node(i+1);

			auto id=lock->get_any_attribute("id");

			if (id.empty())
				throw EXCEPTION(_("no id specified for color"));

			if (colors.find(id) != colors.end())
				continue; // Did this one already.

			auto scale=lock->get_any_attribute("scale");

			rgb color;

			if (!scale.empty())
			{
				auto iter=colors.find(scale);

				if (iter == colors.end())
					continue; // Not yet parsed

				color=iter->second;
			}

			static const char * const channels[]={"r",
							      "g",
							      "b",
							      "a"};
			static uint16_t rgb::* const fields[]={
				&rgb::r,
				&rgb::g,
				&rgb::b,
				&rgb::a};

			for (size_t i=0; i<4; ++i)
			{
				auto attribute=lock->clone();

				auto xpath=attribute->get_xpath(channels[i]);

				if (xpath->count() == 0)
					continue;

				xpath->to_node();

				std::istringstream s(attribute->get_text());

				double v;

				s >> v;

				if (s.fail())
					throw EXCEPTION(gettextmsg(_("could not parse color id=%1%"),
							   id));

				if (v < 0)
					throw EXCEPTION(gettextmsg(_("negative color value for id=%1%"),
							   id));
				uint16_t c;

				if (scale.empty())
				{
					if (v > 1)
						v=1;
					c=v * rgb::maximum;
				}
				else
				{
					v *= color.*(fields[i]);

					if (v > rgb::maximum)
						v=rgb::maximum;
					c=v;
				}

				color.*(fields[i])=c;
			}

			colors.insert(std::make_pair(id, color));
			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (colors.find(id) == colors.end())
			throw EXCEPTION(gettextmsg(_("circular or non-existent dependency of color id=%1%"),
						   id));
	}
}

///////////////////////////////////////////////////////////////////////////////

// Color gradients.

void defaultthemeObj::load_color_gradients(const xml::doc &config)
{
	auto lock=config->readlock();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/color_gradient");

	size_t count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("no id specified for color_gradient"));

		if (color_gradients.find(id) != color_gradients.end())
			continue; // Did this one already.

		auto color=lock->clone();

		auto all_colors=color->get_xpath("color");

		size_t n_colors=all_colors->count();

		rgb::gradient_t new_gradient;

		for (size_t i=0; i<n_colors; ++i)
		{
			all_colors->to_node(i+1);

			rgb::gradient_t::key_type v=0;

			std::istringstream s(color->get_any_attribute("value"));

			s >> v;

			if (s.fail())
				throw EXCEPTION(gettextmsg(_("Missing or invalid @value attribute for color gradient id=%1%"), id));

			auto iter=colors.find(color->get_text());

			if (iter == colors.end())
				throw EXCEPTION(gettextmsg(_("Color gradient id=%1% references non-existent color %2%"), id, color->get_text()));

			if (!new_gradient
			    .insert(std::make_pair(v, iter->second)).second)
				throw EXCEPTION(gettextmsg(_("Duplicate value %1% specified for color gradient id=%1%"), v, id));
		}

		if (new_gradient.find(0) == new_gradient.end())
			throw EXCEPTION(gettextmsg(_("Gradient value 0 specified for color gradient id=%1%"), id));

		color_gradients.insert(std::make_pair(id, new_gradient));
	}
}


/////////////////////////////////////////////////////////////////////////////

dim_t defaultthemeObj::get_theme_dim_t(const std::experimental::string_view &id,
				       dim_t default_value)
{
	auto iter=dims.find(std::string(id.begin(), id.end()));

	if (iter == dims.end())
		return default_value;

	return iter->second;
}

rgb defaultthemeObj::get_theme_color(const std::experimental::string_view &id,
				     const rgb &default_value)
{
	std::vector<std::string> ids;

	if (!id.empty())
		strtok_str(id, ", \r\t\n", ids);

	for (const auto &try_id:ids)
	{
		auto iter=colors.find(try_id);

		if (iter != colors.end())
			return iter->second;
	}

	return default_value;
}

rgb::gradient_t
defaultthemeObj::get_theme_color_gradient(const std::experimental::string_view &id,
					  const rgb::gradient_t &default_value)
{
	std::vector<std::string> ids;

	if (!id.empty())
		strtok_str(id, ", \r\t\n", ids);

	for (const auto &try_id:ids)
	{
		auto iter=color_gradients.find(try_id);

		if (iter != color_gradients.end())
			return iter->second;
	}

	return default_value;
}

//////////////////////////////////////////////////////////////////////////////

LIBCXXW_NAMESPACE_END