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
#include "border_impl.H"
#include "picture.H"
#include "background_color.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/shortcut.H"
#include "gridtemplate.H"
#include <x/property_value.H>
#include <x/chrcasecmp.H>
#include <x/strtok.H>
#include <x/join.H>
#include <x/glob.H>
#include <x/imbue.H>
#include <x/visitor.H>
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

std::pair<std::string, int> connectionObj::current_theme() const
{
	auto theme=impl->screens.at(0)->current_theme.get();

	return {theme->themename, std::round(theme->themescale*100)};
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

	return get_config(themename, themescale);
}

defaulttheme::base::config
defaulttheme::base::get_config(const std::string &themename,
			       double themescale)
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
			theme_configfile };
}

defaultthemeObj::defaultthemeObj(const xcb_screen_t *screen,
				 const config &theme_config)
	: themename(theme_config.themename),
	  themescale(theme_config.themescale),
	  themedir(theme_config.themedir),
	  h1mm(one_millimeter(screen->width_in_pixels,
			      screen->width_in_millimeters, themescale)),
	  v1mm(one_millimeter(screen->height_in_pixels,
			      screen->height_in_millimeters, themescale)),
	  screen(screen)
{
}

void defaultthemeObj::load(const xml::doc &config,
			   const ref<screenObj::implObj> &screen)
{
	try {
		theme_parser_lock lock{config->readlock(), locale::create("C")};

		load_dims(lock);
		load_colors(lock);
		load_color_gradients(lock);
		load_borders(lock, screen);
		load_fonts(lock);
		load_layouts(lock);
		load_factories(lock);
	} catch (const exception &e)
	{
		throw EXCEPTION("An error occured while parsing the "
				<< themename << " theme configuration file: "
				<< e);
	}
}

defaultthemeObj::~defaultthemeObj()=default;

////////////////////////////////////////////////////////////////////////////
//
// Theme parsing functions.

// Look up something optional.

static bool if_given(const theme_parser_lock &lock,
		     const char *xpath_node)
{
	auto xpath=lock->get_xpath(xpath_node);

	return xpath->count() > 0;
}

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

// Parse the dims in the config file

static bool parse_dim(const theme_parser_lock &lock,
		      const std::unordered_map<std::string, dim_t>
		      &existing_dims,
		      dim_t h1mm, dim_t v1mm, dim_t &mm,
		      const char *descr,
		      const std::string &id)
{
	auto scale=lock->get_any_attribute("scale");

	double v;

	{
		auto t=lock->get_text();

		if (t == "inf")
		{
			mm=dim_t::infinite();
			return true;
		}

		std::istringstream i(lock->get_text());

		imbue<std::istringstream> imbue{lock.c_locale, i};

		i >> v;

		if (i.fail())
			throw EXCEPTION(gettextmsg(_("could not parse %1% id=%2%"),
						   descr,
						   id));

		if (v < 0)
			throw EXCEPTION(gettextmsg(_("%1% id=%2% cannot be negative"),
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
			mm=dim_scale(dim_t{1}, v);
			break;
		default:
			mm=dim_scale(h1mm+v1mm, v/2);
			break;
		}
	}

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

// Look up a dimension, when parsing something else.

static bool update_dim_if_given(const theme_parser_lock &lock,
				const char *xpath_node,
				const std::unordered_map<std::string, dim_t
				> &existing_dims,
				dim_t h1mm, dim_t v1mm, dim_t &mm,
				const char *descr,
				const std::string &id)
{
	auto node=lock.clone();

	auto xpath=node->get_xpath(xpath_node);

	if (xpath->count() == 0)
		return false;

	xpath->to_node();

	if (!parse_dim(node, existing_dims, h1mm, v1mm, mm, descr, id))
		unknown_dim(descr, id);

	return true;
}

static bool update_color(const theme_parser_lock &lock,
			 const char *xpath_name,
			 const std::unordered_map<std::string, rgb> &colors,
			 rgb &color)
{
	auto color_node=lock.clone();

	auto xpath=color_node->get_xpath(xpath_name);

	if (xpath->count() == 0)
		return false;

	xpath->to_node();

	auto name=color_node->get_text();

	auto iter=colors.find(name);

	if (iter == colors.end())
		throw EXCEPTION(gettextmsg(_("undefined color: %1%"),
					   name));

	color=iter->second;
	return true;
}

void defaultthemeObj::load_dims(const theme_parser_lock &root_lock)
{
	auto lock=root_lock.clone();

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
			dims.insert({id, mm});
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

void defaultthemeObj::load_colors(const theme_parser_lock &root_lock)
{
	auto lock=root_lock.clone();

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
				auto attribute=lock.clone();

				auto xpath=attribute->get_xpath(channels[i]);

				if (xpath->count() == 0)
					continue;

				xpath->to_node();

				std::istringstream s(attribute->get_text());

				imbue<std::istringstream> imbue{lock.c_locale,
						s};

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

			colors.insert({id, color});
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

void defaultthemeObj::load_color_gradients(const theme_parser_lock &root_lock)
{
	auto lock=root_lock.clone();

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

		auto color=lock.clone();

		auto all_colors=color->get_xpath("color");

		size_t n_colors=all_colors->count();

		rgb_gradient new_gradient;

		for (size_t i=0; i<n_colors; ++i)
		{
			all_colors->to_node(i+1);

			rgb_gradient::key_type v=0;

			std::istringstream s(color->get_any_attribute("value"));

			s >> v;

			if (s.fail())
				throw EXCEPTION(gettextmsg(_("Missing or invalid @value attribute for color gradient id=%1%"), id));

			auto iter=colors.find(color->get_text());

			if (iter == colors.end())
				throw EXCEPTION(gettextmsg(_("Color gradient id=%1% references non-existent color %2%"), id, color->get_text()));

			if (!new_gradient
			    .insert({v, iter->second}).second)
				throw EXCEPTION(gettextmsg(_("Duplicate value %1% specified for color gradient id=%1%"), v, id));
		}

		valid_gradient(new_gradient);

		color_gradients.insert({id, new_gradient});
	}
}

//////////////////////////////////////////////////////////////////////////////
//
// Borders

void defaultthemeObj::load_borders(const theme_parser_lock &root_lock,
				   const ref<screenObj::implObj> &screen)
{
	auto lock=root_lock.clone();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/border");

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
				throw EXCEPTION(_("no id specified for border"));

			if (borders.find(id) != borders.end())
				continue; // Did this one already.

			auto from=lock->get_any_attribute("from");

			bool copied_from=false;

			auto new_border=from.empty()
				? border_impl::create() :
				({
					auto iter=borders.find(from);

					if (iter == borders.end())
						continue; // Not yet parsed

					copied_from=true;

					auto b=iter->second->clone();

					borders.insert({id, b});

					b;
				});

			update_dim_if_given(lock, "width",
					    dims, h1mm, v1mm,
					    new_border->width,
					    "border", id);

			update_dim_if_given(lock, "height",
					    dims, h1mm, v1mm,
					    new_border->height,
					    "border", id);

			// <rounded> sets the radii both to 1.

			if (if_given(lock, "rounded"))
			{
				new_border->hradius=1;
				new_border->vradius=1;
			}

			// Alternatively, hradius and vradius will set them
			// to at least 2.

			if (update_dim_if_given(lock, "hradius",
						dims, h1mm, v1mm,
						new_border->hradius,
						"border", id))
			{
				if (new_border->hradius < 2)
					new_border->hradius=2;
			}

			if (update_dim_if_given(lock, "vradius",
						dims, h1mm, v1mm,
						new_border->vradius,
						"border", id))
			{
				if (new_border->vradius < 2)
					new_border->vradius=2;
			}

			if (new_border->hradius >= 2 ||
			    new_border->vradius >= 2)
			{
				if (new_border->hradius < 2)
					new_border->hradius=2;
				if (new_border->vradius < 2)
					new_border->vradius=2;
			}
			// If we copied the border from another from, then
			// unless the following values are given, don't
			// touch the colors.

			if (!copied_from || if_given(lock, "cellcolor") ||
			    if_given(lock, "nocellcolor") ||
			    if_given(lock, "color"))
			{

				bool cellcolor=false;

				{
					auto node=lock.clone();
					auto xpath=node->get_xpath("cellcolor");

					if (xpath->count())
						cellcolor=true;
				}

				new_border->colors.clear();
				new_border->colors.reserve(2);

				if (!cellcolor)
				{
					rgb color;
					update_color(lock, "color", colors,
						     color);

					new_border->colors.push_back
						(screen->create_background_color
						 (color));

					if (update_color(lock, "color2", colors,
							 color))
					{
						new_border->colors.push_back
							(screen
							 ->create_background_color
							 (color));
					}
				}
			}

			{
				auto dash_nodes=lock.clone();

				auto xpath=dash_nodes->get_xpath("dash");

				size_t n=xpath->count();

				if (n)
				{
					new_border->dashes.clear();
					new_border->dashes.reserve(n);
				}

				for (i=0; i<n; ++i)
				{
					xpath->to_node(i+1);

					dim_t mm;

					if (!parse_dim(dash_nodes, dims,
						       h1mm, v1mm, mm,
						       "dash", id))
						unknown_dim("dash", id);

					new_border->dashes
						.push_back((dim_t::value_type)
							   mm);
				}
			}
			new_border->calculate();
			borders.insert({id, new_border});
			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (borders.find(id) == borders.end())
			throw EXCEPTION(gettextmsg(_("circular or non-existent dependency of border id=%1%"),
						   id));
	}
}

/////////////////////////////////////////////////////////////////////////////

dim_t defaultthemeObj::get_theme_width_dim_t(const dim_arg &id)
{
	return std::visit(visitor{
			[this](double v)
			{
				return compute_height(v);
			},
			[&](const std::string &id)
			{
				auto iter=dims.find(id);

				if (iter == dims.end())
					throw EXCEPTION
						(gettextmsg
						 (_("Size %1% does not exist"),
						  id));
				return iter->second;
			}}, id);
}

dim_t defaultthemeObj::get_theme_height_dim_t(const dim_arg &id)
{
	return std::visit(visitor{
			[this](double v)
			{
				return compute_height(v);
			},
			[&](const std::string &id)
			{
				auto iter=dims.find(id);

				if (iter == dims.end())
					throw EXCEPTION
						(gettextmsg
						 (_("Size %1% does not exist"),
						  id));
				return iter->second;
			}}, id);
}

dim_t defaultthemeObj::compute_width(double millimeters)
{
	if (std::isnan(millimeters))
		return dim_t::infinite();

	if (millimeters < 0)
		millimeters= -millimeters;

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

dim_t defaultthemeObj::compute_height(double millimeters)
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
		auto iter=colors.find(try_id);

		if (iter != colors.end())
			return iter->second;
	}

	throw EXCEPTION(gettextmsg(_("Theme color %1% does not exist"), id));
}

rgb_gradient
defaultthemeObj::get_theme_color_gradient(const rgb_gradient_arg &arg) const
{
	return std::visit(visitor{
			[](const rgb_gradient &g) { return g; },
			[this](const std::string &id)
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

				throw EXCEPTION
					(gettextmsg
					 (_("Theme gradient %1% "
					    "does not exist"), id));
			}}, arg);
}


const_border_impl
defaultthemeObj::get_theme_border(const std::string_view &id)
{
	auto iter=borders.find(std::string(id));

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

font defaultthemeObj::get_theme_font(const std::string &id)
{
	auto iter=fonts.find(id);

	if (iter != fonts.end())
		return iter->second;

	throw EXCEPTION(gettextmsg(_("Theme font %1% does not exist"),
				   id));
}

//////////////////////////////////////////////////////////////////////////////


static std::string single_value(const theme_parser_lock &lock,
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

static std::string optional_value(const theme_parser_lock &lock,
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

static std::string lowercase_single_value(const theme_parser_lock &lock,
					  const char *element,
					  const char *xpath)
{
	std::string s=single_value(lock, element, xpath);

	std::transform(s.begin(), s.end(), s.begin(), chrcasecmp::tolower);

	return s;
}

static bool single_value_exists(const theme_parser_lock &lock,
				const char *element)
{
	auto v=lock.clone();

	auto xpath=v->get_xpath(element);

	return xpath->count() == 1;
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

inline static dim_t to_dim_t(const theme_parser_lock &lock,
			     const char *element, const char *parent)
{
	return to_value<dim_t>(lock,
			       single_value(lock, element, parent), element);
}

inline static size_t to_size_t(const theme_parser_lock &lock,
			       const char *element, const char *parent)
{
	return to_value<size_t>(lock,
				single_value(lock, element, parent), element);
}

inline static int to_percentage_t(const theme_parser_lock &lock,
				  const char *element, const char *parent)
{
	int v=to_value<int>(lock,
			    single_value(lock, element, parent), element);

	if (v < 0 || v > 100)
		throw EXCEPTION(gettextmsg(_("Value of <%1%> is not 0-100"
					     ), element));

	return v;
}

static halign to_halign_value(const theme_parser_lock &lock,
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

static valign to_valign_value(const theme_parser_lock &lock,
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

#include "gridlayoutapi.inc.C"

void defaultthemeObj::load_layouts(const theme_parser_lock &root_lock)
{
	auto lock=root_lock.clone();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/layout");

	size_t count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("no id specified for layout"));

		if (gridlayouts.find(id) != gridlayouts.end())
			continue;

		if (booklayouts.find(id) != booklayouts.end())
			continue;

		auto type=lock->get_any_attribute("type");

		if (type == "book")
		{
			booklayout_parseconfig(lock, booklayouts[id]);
		}
		else if (type == "grid")
		{
			gridlayout_parseconfig(lock, gridlayouts[id]);
		}
		else
		{
			throw EXCEPTION("Unknown type="
					<< type
					<< "for layout id=" << id);
		}
	}
}

void defaultthemeObj::load_factories(const theme_parser_lock &root_lock)
{
	auto lock=root_lock.clone();

	if (!lock->get_root())
		return;

	auto xpath=lock->get_xpath("/theme/factory");

	size_t count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("no id specified for factory"));

		if (gridfactories.find(id) != gridfactories.end())
			continue;

		if (bookpagefactories.find(id) != bookpagefactories.end())
			continue;

		auto type=lock->get_any_attribute("type");

		if (type == "book")
		{
			bookpagefactory_parseconfig(lock,
						    bookpagefactories[id]);
		}
		else if (type == "grid")
		{
			gridfactory_parseconfig(lock, gridfactories[id]);
		}
		else
		{
			throw EXCEPTION("Unknown type="
					<< type
					<< "for factory id=" << id);
		}
	}
}

void defaultthemeObj::layout_append_row(const gridlayoutmanager &glm,
					const gridtemplate *elements,
					const std::string &name)
{
	auto iter=gridfactories.find(name);

	if (iter == gridfactories.end())
		throw EXCEPTION
			(gettextmsg
			 (_("Did not find definition of \"%1%\" in the theme"),
			  name));

	auto f=glm->append_row();
	auto me=defaulttheme(this);

	for (const auto &c:iter->second)
		c(f, elements, me);
}

void defaultthemeObj::layout_insert(const factory &f,
				    const gridtemplate *elements,
				    const std::string &name,
				    const std::string &background_color)
{
	f->create_container([&, this]
			    (const auto &new_container)
			    {
				    gridlayoutmanager glm=
					    new_container->get_layoutmanager();
				    if (!background_color.empty())
					    new_container->set_background_color
						    (background_color);

				    elements->new_layouts
					    .emplace(name,
						     new_container);

				    this->layout_insert(glm, elements, name);
			    },
			    new_gridlayoutmanager{});
}

void defaultthemeObj::layout_book_container(const factory &f,
					    const gridtemplate *elements,
					    const std::string &name,
					    const std::string &background_color,
					    const std::string &border)
{
	new_booklayoutmanager nblm;

	if (!background_color.empty())
		nblm.background_color=background_color;

	if (!border.empty())
		nblm.border=border;

	f->create_focusable_container
		([&, this]
		 (const auto &new_container)
		 {
			 elements->new_layouts.emplace(name,
						       new_container);
			 booklayoutmanager blm=
				 new_container->get_layoutmanager();

			 this->layout_book_container(blm, elements,
						     name);
		 },
		 nblm);
}

void defaultthemeObj::layout_insert(const gridlayoutmanager &glm,
				    const gridtemplate *elements,
				    const std::string &name)
{
	auto iter=gridlayouts.find(name);

	if (iter == gridlayouts.end())
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));

	auto me=defaulttheme(this);

	for (const auto &c:iter->second)
		c(glm, elements, me);
}


void defaultthemeObj::layout_book_container(const booklayoutmanager &blm,
					    const gridtemplate *elements,
					    const std::string &name)
{
	auto iter=booklayouts.find(name);

	if (iter == booklayouts.end())
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));

	auto me=defaulttheme(this);

	for (const auto &c:iter->second)
		c(blm, elements, me);
}

void defaultthemeObj::layout_append_pages(const booklayoutmanager &blm,
					  const gridtemplate *elements,
					  const std::string &name)
{
	auto f=blm->append();

	auto iter=bookpagefactories.find(name);

	if (iter == bookpagefactories.end())
		throw EXCEPTION(gettextmsg(_("Book factory %1% not defined."),
					   name));

	auto me=defaulttheme(this);

	for (const auto &c:iter->second)
		c(f, elements, me);
}

void defaultthemeObj::layout_add_page(const bookpagefactory &f,
				      const gridtemplate *elements,
				      const std::string &label,
				      const std::string &sc,
				      const std::string &name)
{
	auto shortcut_iter=elements->shortcuts.find(sc);

	f->add([&, this]
	       (const auto &label_factory,
		const auto &page_factory)
	       {
		       elements->generate(label_factory, label);

		       page_factory->create_container
			       ([&]
				(const auto &container)
				{
					gridlayoutmanager glm=
						container->get_layoutmanager();

					layout_insert(glm, elements, name);
				},
				new_gridlayoutmanager{});
	       },
	       shortcut_iter == elements->shortcuts.end()
	       ? shortcut{}:shortcut_iter->second);
}

LIBCXXW_NAMESPACE_END
