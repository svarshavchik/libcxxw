/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "defaulttheme.H"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/book_appearance.H"
#include "x/w/shortcut.H"
#include "theme_parser_lock.H"
#include "messages.H"
#include "picture.H"
#include <x/functionalrefptr.H>
#include <x/visitor.H>

LIBCXXW_NAMESPACE_START


const char * const rgb_channels[]={"r",
				   "g",
				   "b",
				   "a"};

rgb_component_t rgb::* const rgb_fields[]=
	{
	 &rgb::r,
	 &rgb::g,
	 &rgb::b,
	 &rgb::a};



static inline bool scale_theme_color(theme_parser_lock &lock,
				     const std::string &id,
				     const std::string &scale,
				     rgb &color,
				     std::unordered_map<std::string,
				     theme_color_t> &parsed_colors)
{
	std::istringstream s;

	imbue i_parse{lock.c_locale, s};

	for (size_t i=0; i<4; ++i)
	{
		auto attribute=lock.clone();

		auto xpath=attribute->get_xpath(rgb_channels[i]);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		s.seekg(0);
		s.str(attribute->get_text());

		double v;

		s >> v;

		if (s.fail())
			throw EXCEPTION(gettextmsg(_("could not parse color id=%1%"),
						   id));

		if (v < 0)
			throw EXCEPTION(gettextmsg(_("negative color value for id=%1%"),
						   id));
		rgb_component_t c;

		if (scale.empty())
		{
			if (v > 1)
				v=1;
			c=v * rgb::maximum;
		}
		else
		{
			v *= color.*(rgb_fields[i]);

			if (v > rgb::maximum)
				v=rgb::maximum;
			c=v;
		}

		color.*(rgb_fields[i])=c;
	}

	return true;
}

static bool parse_gradients(theme_parser_lock &lock,
			    rgb_gradient &gradient,
			    std::unordered_map<std::string,
			    theme_color_t> &parsed_colors,
			    const std::string &id);

static inline bool scale_theme_color(theme_parser_lock &lock,
				     const std::string &id,
				     const std::string &scale,
				     linear_gradient &color,
				     std::unordered_map<std::string,
				     theme_color_t> &parsed_colors)
{
	static const char * const coords[]={"x1",
					    "y1",
					    "x2",
					    "y2",
					    "widthmm",
					    "heightmm"};

	static const double minvalue[]={0,0,0,0,-999,-999};
	static const double maxvalue[]={1,1,1,1,999,999};

	static double linear_gradient::* const fields[]={
		&linear_gradient::x1,
		&linear_gradient::y1,
		&linear_gradient::x2,
		&linear_gradient::y2,
		&linear_gradient::fixed_width,
		&linear_gradient::fixed_height};

	std::istringstream s(lock->get_text());

	imbue i_parse{lock.c_locale, s};

	for (size_t i=0; i<6; i++)
	{
		auto attribute=lock.clone();

		auto xpath=attribute->get_xpath(coords[i]);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		s.seekg(0);
		s.str(attribute->get_text());

		double v;

		s >> v;

		if (s.fail())
			throw EXCEPTION(gettextmsg
					(_("could not parse %2% for id=%1%"),
					 id, coords[i]));

		if (v < minvalue[i] || v>maxvalue[i])
			throw EXCEPTION(gettextmsg
					(_("%2% for id=%1% must be between %3%"
					   " and %4%"),
					 id, coords[i],
					 minvalue[i], maxvalue[i]));

		color.*(fields[i])=v;
	}

	bool flag;

	try {
		flag=parse_gradients(lock, color.gradient, parsed_colors, id);
	} catch (const exception &e) {
		std::ostringstream o;

		o << e;

		throw EXCEPTION(gettextmsg
				(_("gradient id=%1% is not valid: %2%"),
				 id, o.str()));
	}

	return flag;
}

static inline bool scale_theme_color(theme_parser_lock &lock,
				     const std::string &id,
				     const std::string &scale,
				     radial_gradient &color,
				     std::unordered_map<std::string,
				     theme_color_t> &parsed_colors)
{
	static const char * const coords[]={"inner_x",
					    "inner_y",
					    "outer_x",
					    "outer_y",
					    "inner_radius",
					    "outer_radius",
					    "widthmm",
					    "heightmm"};

	static const double minvalue[]={0,0,0,0,0,0,-999,-999};
	static const double maxvalue[]={1,1,1,1,999,999,999,999};

	static double radial_gradient::* const fields[]={
		&radial_gradient::inner_center_x,
		&radial_gradient::inner_center_y,
		&radial_gradient::outer_center_x,
		&radial_gradient::outer_center_y,
		&radial_gradient::inner_radius,
		&radial_gradient::outer_radius,
		&radial_gradient::fixed_width,
		&radial_gradient::fixed_height};

	std::istringstream s;

	imbue i_parse{lock.c_locale, s};

	for (size_t i=0; i<6; i++)
	{
		auto attribute=lock.clone();

		auto xpath=attribute->get_xpath(coords[i]);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		s.seekg(0);
		s.str(attribute->get_text());

		double v;

		s >> v;

		if (s.fail())
			throw EXCEPTION(gettextmsg
					(_("could not parse %2% for id=%1%"),
					 id, coords[i]));

		if (v < minvalue[i] || v>maxvalue[i])
			throw EXCEPTION(gettextmsg
					(_("%2% for id=%1% must be between %3%"
					   " and %4%"),
					 id, coords[i],
					 minvalue[i], maxvalue[i]));

		color.*(fields[i])=v;
	}

	static const char * const raxises[]={"inner_radius_axis",
					     "outer_radius_axis"};

	static radial_gradient::radius_axis radial_gradient::* const rfields[]={
		&radial_gradient::inner_radius_axis,
		&radial_gradient::outer_radius_axis};

	for (size_t i=0; i<2; i++)
	{
		if (!single_value_exists(lock, raxises[i]))
			continue;

		auto s=lowercase_single_value(lock, raxises[i], "color");

		if (s.empty())
			continue;

		if (s == "horizontal")
		{
			color.*(rfields[i])=radial_gradient::horizontal;
		}
		else if (s == "vertical")
		{
			color.*(rfields[i])=radial_gradient::vertical;
		}
		else if (s == "shortest")
		{
			color.*(rfields[i])=radial_gradient::shortest;
		}
		else if (s == "longest")
		{
			color.*(rfields[i])=radial_gradient::longest;
		}
		else
			throw EXCEPTION(gettextmsg
					(_("%1% is not a valid value for %2%"
					   ", id=%3%"),
					 s, raxises[i], id));
	}

	bool flag;

	try {
		flag=parse_gradients(lock, color.gradient, parsed_colors, id);
	} catch (const exception &e) {
		std::ostringstream o;

		o << e;

		throw EXCEPTION(gettextmsg
				(_("gradient id=%1% is not valid: %2%"),
				 id, o.str()));
	}

	return flag;
}

static bool parse_gradients(theme_parser_lock &lock,
			    rgb_gradient &parsed_gradient,
			    std::unordered_map<std::string,
			    theme_color_t> &parsed_colors,
			    const std::string &id)
{
	auto gradients=lock.clone();

	auto xpath=gradients->get_xpath("gradient");

	size_t n=xpath->count();

	std::istringstream s;

	imbue i_parse{lock.c_locale, s};

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		auto gradient=gradients.clone();

		auto vxpath=gradient->get_xpath("value");

		vxpath->to_node();

		unsigned n;

		s.seekg(0);
		s.str(gradient->get_text());

		s >> n;

		if (s.fail())
			throw EXCEPTION(gettextmsg
					(_("could not parse <value>"
					   " for id=%1%"),
					 id));

		gradient=gradients.clone();
		vxpath=gradient->get_xpath("color");
		vxpath->to_node();

		auto s=gradient->get_text();

		if (s.empty())
			continue;

		auto iter=parsed_colors.find(s);

		if (iter == parsed_colors.end())
			return false; // Not yet parsed

		if (!std::holds_alternative<rgb>(iter->second))
			throw EXCEPTION(gettextmsg
					(_("gradient id=%1% cannot use color "
					   "%2"), id, s));
		parsed_gradient.emplace(n, std::get<rgb>(iter->second));
	}

	valid_gradient(parsed_gradient);
	return true;
}


static std::optional<color_arg>
get_color(const theme_parser_lock &lock,
	  const char *xpath_name,
	  const uigeneratorsObj &generators,
	  bool allowthemerefs)
{
	auto color_node=lock.clone();

	auto xpath=color_node->get_xpath(xpath_name);

	if (xpath->count() == 0)
		return std::nullopt;

	xpath->to_node();

	auto name=color_node->get_text();

	return generators.lookup_color(name, allowthemerefs, xpath_name);
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

static void update_dim_if_given(const theme_parser_lock &lock,
				const char *size_node,
				const char *scale_node,
				dim_arg &size,
				unsigned &scale,
				const char *descr,
				const std::string &id,
				const uigeneratorsObj &generators,
				bool allowthemerefs)
{
	if (single_value_exists(lock, size_node))
	{
		auto t=single_value(lock, size_node, descr);

		std::istringstream i{t};

		double v;

		i >> v;

		if (i.fail())
		{
			size=generators.lookup_dim(t, allowthemerefs, descr);
		}
		else
		{
			size=v;
		}
	}

	if (single_value_exists(lock, scale_node))
	{
		auto t=single_value(lock, scale_node, descr);

		std::istringstream i{t};

		// The contents of this node must be a scaling factor.
		i >> scale;

		if (i.fail())
			throw EXCEPTION(gettextmsg(_("Cannot parse %1% (%2%)"),
						   descr, id));
	}
}


// Parse the dims in the config file

static inline std::optional<double>
parse_dim(const theme_parser_lock &lock,
	  const std::unordered_map<std::string, double> &existing_dims,
	  const char *descr,
	  const std::string &id)
{
	auto scale=lock->get_any_attribute("scale");

	double v;

	{
		auto t=lock->get_text();

		if (t == "inf")
		{
			return NAN;
		}

		std::istringstream i(lock->get_text());

		imbue i_parse{lock.c_locale, i};

		i >> v;

		if (i.fail())
			throw EXCEPTION(gettextmsg(_("could not parse %1% id=%2%"),
						   descr,
						   id));

		if (v < 0)
			throw EXCEPTION(gettextmsg(_("%1% id=%2% cannot be negative"),
						   descr,
						   id));

		char p=0;

		if (i >> p && p == 'p')
			v= -v;
	}

	if (!scale.empty())
	{
		auto iter=existing_dims.find(scale);

		if (iter == existing_dims.end())
			return std::nullopt; // Not yet.

		v *= iter->second;
	}

	return v;
}


uicompiler::uicompiler(const theme_parser_lock &root_lock,
		       uigeneratorsObj &generators,
		       bool allowthemerefs)
	: generators{generators}, allowthemerefs{allowthemerefs}
{
	if (!root_lock->get_root())
		return;

	auto lock=root_lock.clone();

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

			if (generators.colors.find(id) !=
			    generators.colors.end())
				continue; // Did this one already.

			auto scale=lock->get_any_attribute("scale");

			theme_color_t new_color;

			if (!scale.empty())
			{
				auto iter=generators.colors.find(scale);

				if (iter == generators.colors.end())
					continue; // Not yet.

				new_color=iter->second;
			}
			else
			{
				auto type=lock->get_any_attribute("type");

				if (type.empty()) type="rgb";

				if (type != "rgb" && type != "linear_gradient"
				    && type != "radial_gradient")
					throw EXCEPTION
						(gettextmsg
						 (_("Unknown type=%1% specified"
						    " for id=%2%"),
						  type, id));
				if (type == "linear_gradient")
					new_color=linear_gradient{};
				if (type == "radial_gradient")
					new_color=radial_gradient{};
			}

			bool flag=std::visit
				([&]
				 (auto &c)
				 {
					 return scale_theme_color
						 (lock, id,
						  scale, c,
						  generators.colors);
				 }, new_color);

			if (flag)
			{
				generators.colors.insert({id, new_color});
				parsed=true;
			}

		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (generators.colors.find(id) != generators.colors.end())
			continue;

		throw EXCEPTION(gettextmsg
				(_("Color %1% is based on another"
				   " color which was not found"
				   " (this can be because of a circular"
				   " reference)"),
				 id));
	}

	lock=root_lock.clone();

	xpath=lock->get_xpath("/theme/dim");

	count=xpath->count();

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

			if (generators.dims.find(id) != generators.dims.end())
				continue; // Did this one already.

			auto value=parse_dim(lock,
					     generators.dims, "dim", id);

			if (!value)
				continue;

			generators.dims.insert({id, *value});
			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (generators.dims.find(id) == generators.dims.end())
			unknown_dim("dim", id);
	}

	lock=root_lock.clone();

	xpath=lock->get_xpath("/theme/border");

	count=xpath->count();

	// Repeatedly pass over all borders, parsing the ones that are not based
	// on unparsed borders.

	do
	{
		parsed=false;

		for (size_t i=0; i<count; ++i)
		{
			xpath->to_node(i+1);

			auto id=lock->get_any_attribute("id");

			if (id.empty())
				throw EXCEPTION(_("no id specified for border"));

			if (generators.borders.find(id) !=
			    generators.borders.end())
				continue; // Did this one already.

			border_infomm new_border;

			auto from=lock->get_any_attribute("from");

			bool created_border=true;

			if (!from.empty())
			{
				auto iter=generators.borders.find(from);

				if (iter == generators.borders.end())
					continue; // Not yet parsed

				new_border=iter->second;
				created_border=false;
			}

			// If we copied the border from another from, then
			// unless the following values are given, don't
			// touch the colors.

			auto color1=get_color(lock, "color", generators,
					      allowthemerefs);

			if (color1)
			{
				new_border.color1= *color1;

				new_border.color2=get_color(lock, "color2",
							    generators,
							    allowthemerefs);
			}
			else if (created_border)
			{
				throw EXCEPTION(gettextmsg
						(_("<color> not specified for "
						   "%1"), id));
			}

			update_dim_if_given(lock, "width", "width_scale",
					    new_border.width,
					    new_border.width_scale,
					    "border", id,
					    generators,
					    allowthemerefs);

			update_dim_if_given(lock, "height", "height_scale",
					    new_border.height,
					    new_border.height_scale,
					    "border", id,
					    generators,
					    allowthemerefs);

			// <rounded> sets the radii both to 1.

			if (single_value_exists(lock, "rounded"))
			{
				auto rounded=single_value(lock, "rounded",
							  "border");

				new_border.rounded=rounded != "0";
			}

			// Alternatively, hradius and vradius will set them
			// to at least 2.

			update_dim_if_given(lock, "hradius", "hradius_scale",
					    new_border.hradius,
					    new_border.hradius_scale,
					    "border", id,
					    generators,
					    allowthemerefs);

			update_dim_if_given(lock, "vradius", "vradius_scale",
					    new_border.vradius,
					    new_border.vradius_scale,
					    "border", id,
					    generators,
					    allowthemerefs);

			{
				auto dash_nodes=lock.clone();

				auto xpath=dash_nodes->get_xpath("dash");

				size_t n=xpath->count();

				if (n)
				{
					new_border.dashes.clear();
					new_border.dashes.reserve(n);
				}

				for (i=0; i<n; ++i)
				{
					xpath->to_node(i+1);

					dim_t mm;

					auto t=dash_nodes->get_text();

					if (t.empty())
						continue;

					std::istringstream i{t};

					imbue i_parse{dash_nodes.c_locale, i};

					double v;

					i >> v;

					if (i.fail())
						throw EXCEPTION
							(gettextmsg
							 (_("Cannot parse dash "
							    "values of border "
							    "%1%"), id));

					new_border.dashes.push_back(v);
				}
			}

			generators.borders.emplace(id, new_border);
			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (generators.borders.find(id) != generators.borders.end())
			continue;

		throw EXCEPTION(gettextmsg
				(_("Border %1% is based on another"
				   " color which was not found"
				   " (this can be because of a circular"
				   " reference)"),
				 id));
	}

	load_fonts(root_lock.clone(),
		   [&]
		   (const std::string &id, const font &new_font)
		   {
			   generators.fonts.emplace(id, new_font);
		   },
		   [&]
		   (const std::string &from) -> std::optional<font>
		   {
			   auto iter=generators.fonts.find(from);

			   if (iter == generators.fonts.end())
				   return std::nullopt;

			   return iter->second;
		   });

	xpath=lock->get_xpath("/theme/layout | /theme/factory");

	// Build the list of uncompiled_elements, by id.

	count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("Missing \"id\" element"));

		uncompiled_elements.emplace(id, lock->clone());
	}

	// Keep compiling, one at a time, until all done.

	while (!uncompiled_elements.empty())
	{
		auto first=uncompiled_elements.begin();

		auto lock=first->second;
		auto name=first->second->name();
		auto type=first->second->get_any_attribute("type");
		auto id=first->first;

		uncompiled_elements.erase(first);

		if (name == "layout")
		{
			if (type == "grid")
			{
				auto ret=gridlayout_parseconfig(lock);
				generators.gridlayoutmanager_generators
					.emplace(id, ret);
				continue;
			}

			if (type == "book")
			{
				auto ret=booklayout_parseconfig(lock);
				generators.booklayoutmanager_generators
					.emplace(id, ret);
				continue;
			}
		}
		else if (name == "factory")
		{
			if (type == "grid")
			{
				auto ret=gridfactory_parseconfig(lock);
				generators.gridfactory_generators
					.emplace(id, ret);
				continue;
			}

			if (type == "book")
			{
				auto ret=bookpagefactory_parseconfig(lock);
				generators.bookpagefactory_generators
					.emplace(id, ret);
				continue;
			}
		}

		throw EXCEPTION(gettextmsg(_("Unrecognized %1% type \"%2%\""),
					   name,
					   type));
	}
}

void uicompiler::do_load_fonts(const theme_parser_lock &lock,
			       const function<void (const std::string &,
						    const font &)> &install,
			       const function<std::optional<font>
			       (const std::string &)> &lookup)
{
	auto xpath=lock->get_xpath("font");

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

			if (lookup(id))
				continue; // Did this one already.

			font new_font;

			auto from=lock->get_any_attribute("from");

			if (!from.empty())
			{
				auto ret=lookup(from);

				if (!ret)
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

				imbue i_parse{lock.c_locale, i};

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

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (!lookup(id))
			throw EXCEPTION(gettextmsg(_("circular or non-existent dependency of font id=%1%"),
						   id));
	}
}

void uicompiler::generate(const factory &f,
			  uielements &uif,
			  const std::string &name)
{
	auto iter=uif.factories.find(name);

	if (iter == uif.factories.end())
		throw EXCEPTION(gettextmsg(_("Element \"%1%\" not defined."),
					   name));

	iter->second(f);
}

///////////////////////////////////////////////////////////////////////////
//
// Find a generator for a layout manager or a factory.
//
// Look in the compiled generators first. If not found, it must be in
// uncompiled_elements. Find it, recursively compile it now, put it into
// generators, and return it.
//
// In this manner we end up compiling any generators that are referenced
// from other generators recursively, and because we remove the definition
// from uncompiled_elements beforehand, this detects and fails infinite
// recursion.
//
// This is called from the stylesheet-generator parser upon encountering
// an element that references a generator, by name. This ends up effectively
// translating the named reference in the stylesheet into a compiled generator.
// The referenced element then gets passed as a parameter to something like
// container_gridlayoutmanager, which ends up
// invokes something that executes the generator, when it runs, like
// container_gridlayoutmanager that creates a new container with the grid
// layout manager, then executes the generator for it.
//
// A factory reference ends up

uicompiler::container_generators_t
uicompiler::lookup_container_generators(const std::string &type,
					const theme_parser_lock &lock,
					const std::string &name,
					bool,
					const char *tag)
{
	if (type == "grid")
	{
		return lookup_gridlayoutmanager_generators(lock, name);
	}

	if (type == "book")
	{
		return lookup_booklayoutmanager_generators(lock, name);
	}

	throw EXCEPTION(gettextmsg(_("The %1% <type> is not valid for %2%"),
				   type, tag));
}

vector<gridlayoutmanager_generator>
uicompiler::lookup_gridlayoutmanager_generators(const theme_parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators.gridlayoutmanager_generators.find(name);

		if (iter != generators.gridlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "layout"
	    || iter->second->get_any_attribute("type") != "grid")
	{
		throw EXCEPTION(gettextmsg(_("Layout \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=gridlayout_parseconfig(new_lock);

	generators.gridlayoutmanager_generators.emplace(name, ret);

	return ret;
}

vector<booklayoutmanager_generator>
uicompiler::lookup_booklayoutmanager_generators(const theme_parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators.booklayoutmanager_generators.find(name);

		if (iter != generators.booklayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "layout"
	    || iter->second->get_any_attribute("type") != "book")
	{
		throw EXCEPTION(gettextmsg(_("Book layout \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=booklayout_parseconfig(new_lock);

	generators.booklayoutmanager_generators.emplace(name, ret);

	return ret;
}

void uicompiler::create_container(const factory &f,
				  uielements &factories,
				  const std::string &name,
				  const container_generators_t &generators)
{
	std::visit
		(visitor
		 {[&](const vector<gridlayoutmanager_generator> &generators)
		  {
			  f->create_container
				  ([&]
				   (const auto &new_container)
				   {
					   factories.new_layouts
						   .emplace(name,
							    new_container);

					   gridlayoutmanager glm=
						   new_container
						   ->get_layoutmanager();

					   for (const auto &g:*generators)
					   {
						   g(glm, factories);
					   }
				   },
				   new_gridlayoutmanager{});
		  },
		  [&](const vector<booklayoutmanager_generator> &generators)
		  {
			  new_booklayoutmanager nblm;

			  f->create_focusable_container
				  ([&]
				   (const auto &new_container)
				   {
					   factories.new_layouts
						   .emplace(name,
							    new_container);

					   booklayoutmanager blm=
						   new_container
						   ->get_layoutmanager();

					   for (const auto &g:*generators)
					   {
						   g(blm, factories);
					   }
				   },
				   nblm);
		  }
		 }, generators);

}

vector<gridfactory_generator>
uicompiler::lookup_gridfactory_generators(const theme_parser_lock &lock,
					  const char *element,
					  const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators.gridfactory_generators.find(name);

		if (iter != generators.gridfactory_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "factory"
	    || iter->second->get_any_attribute("type") != "grid")
	{
		throw EXCEPTION(gettextmsg(_("Factory \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=gridfactory_parseconfig(new_lock);

	generators.gridfactory_generators.emplace(name, ret);

	return ret;
}

vector<bookpagefactory_generator>
uicompiler::lookup_bookpagefactory_generators(const theme_parser_lock &lock,
					      const char *element,
					      const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators.bookpagefactory_generators.find(name);

		if (iter != generators.bookpagefactory_generators.end())
			return iter->second;
	}

	auto iter=uncompiled_elements.find(name);

	if (iter == uncompiled_elements.end()
	    || iter->second->name() != "factory"
	    || iter->second->get_any_attribute("type") != "book")
	{
		throw EXCEPTION(gettextmsg(_("Factory \"%1%\", "
					     "does not exist, or is a part of "
					     "an infinitely-recursive layout"),
					   name));
	}

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=bookpagefactory_parseconfig(new_lock);

	generators.bookpagefactory_generators.emplace(name, ret);

	return ret;
}

void uicompiler::gridlayout_append_row(const gridlayoutmanager &layout,
				       uielements &factories,
				       const vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->append_row(), factories, g);
}

void uicompiler::gridlayout_insert_row(const gridlayoutmanager &layout,
				       size_t row,
				       uielements &factories,
				       const vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->insert_row(row), factories, g);
}

void uicompiler::gridlayout_replace_row(const gridlayoutmanager &layout,
					size_t row,
					uielements &factories,
					const vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->replace_row(row), factories, g);
}

void uicompiler::gridlayout_append_columns(const gridlayoutmanager &layout,
					   size_t row,
					   uielements &factories,
					   const
					   vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->append_columns(row), factories, g);
}

void uicompiler::gridlayout_insert_columns(const gridlayoutmanager &layout,
					   size_t row,
					   size_t col,
					   uielements &factories,
					   const
					   vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->insert_columns(row, col), factories, g);
}

void uicompiler::gridlayout_replace_cell(const gridlayoutmanager &layout,
					 size_t row,
					 size_t col,
					 uielements &factories,
					 const
					 vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->replace_cell(row, col), factories, g);
}

void uicompiler::generate_gridfactory(const gridfactory &f,
				      uielements &factories,
				      const vector<gridfactory_generator>
				      &generators)
{
	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler::booklayout_append_pages(const booklayoutmanager &blm,
					 uielements &factories,
					 const vector<bookpagefactory_generator>
					 &generators)
{
	auto f=blm->append();

	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler::booklayout_insert_pages(const booklayoutmanager &blm,
					 size_t pos,
					 uielements &factories,
					 const vector<bookpagefactory_generator>
					 &generators)
{
	auto f=blm->insert(pos);

	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler::create_container(const bookpagefactory &f,
				  uielements &factories,
				  const std::string &label,
				  const std::string &sc,
				  const std::string &name,
				  const container_generators_t &generators)
{
	auto shortcut_iter=factories.shortcuts.find(sc);

	f->add([&]
	       (const auto &label_factory,
		const auto &page_factory)
	       {
		       generate(label_factory, factories, label);

		       create_container(page_factory, factories, name,
					generators);
	       },
	       shortcut_iter == factories.shortcuts.end()
	       ? shortcut{}:shortcut_iter->second);
}

#include "uicompiler.inc.C"

LIBCXXW_NAMESPACE_END
