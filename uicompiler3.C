/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "uicompiler.H"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/text_param.H"
#include "x/w/theme_text.H"
#include "x/w/shortcut.H"
#include "x/w/appearance.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/panefactory.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/panefactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/pagelayoutmanager.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/impl/uixmlparser.H"
#include <x/functional.H>
#include <x/visitor.H>
#include <x/xml/xpath.H>
#include <x/imbue.H>
#include <x/visitor.H>
#include <x/value_string.H>
#include "picture.H"
#include "messages.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

static void unknown_dim(const std::string &id)
       __attribute__((noreturn));

static void unknown_dim(const std::string &id)
{
	throw EXCEPTION(gettextmsg(_("circular or non-existent dependency"
				     " of dim %1%"),
				   id));
}

// Parse the dims in the config file
//! Parsed <dim> value in the UI theme file

struct parsed_dim {
	//! Value of the scale attribute, if not empty.
	std::string scale;

	//! <dim>'s value.
	std::string value;
};

ui::parsed_dim ui::parse_dim(const xml::readlock &lock)
{
	parsed_dim v{lock->get_any_attribute("scale"),
		     lock->get_text()};

	return v;
}

static inline std::optional<double>
parse_dim_value(const ui::parser_lock &lock,
		const std::unordered_map<std::string, double> &existing_dims,
		const char *descr,
		const std::string &id)
{
	auto parsed_dim=ui::parse_dim(lock);

	double v;

	if (parsed_dim.value == "inf")
	{
		return NAN;
	}

	std::istringstream i{parsed_dim.value};

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

	if (!parsed_dim.scale.empty())
	{
		auto iter=existing_dims.find(parsed_dim.scale);

		if (iter == existing_dims.end())
			return std::nullopt; // Not yet.

		v *= iter->second;
	}

	return v;
}

struct LIBCXX_HIDDEN parse_gradient_values;

struct parse_gradient_values {

	rgb_gradient &gradient;
	std::unordered_map<std::string, theme_color_t> &parsed_colors;
	bool &all_found;

	parse_gradient_values(rgb_gradient &gradient,
			      std::unordered_map<std::string, theme_color_t
			      > &parsed_colors,
			      bool &all_found)
		: gradient{gradient},
		  parsed_colors{parsed_colors},
		  all_found{all_found}
	{
		all_found=true;
	}

	void operator()(size_t n, const std::string &v)
	{
		auto iter=parsed_colors.find(v);

		if (iter == parsed_colors.end())
		{
			all_found=false;
			return; // Not yet parsed
		}

		if (!std::holds_alternative<rgb>(iter->second))
			throw EXCEPTION(gettextmsg
					(_("gradient cannot use color "
					   "%1%, only an rgb color"),
					 v));

		gradient.insert_or_assign(n, std::get<rgb>(iter->second));
	}
};

static inline bool
parse_color(const ui::parser_lock &lock,
	    const std::string &id,
	    std::unordered_map<std::string, theme_color_t> &parsed_colors)
{
	auto parsed_color=ui::parse_color(lock);

	return std::visit
		(visitor
		 {[&]
		  (const rgb &c)
		  {
			  parsed_colors.insert_or_assign(id, c);
			  return true;
		  },
		  [&]
		  (const ui::parsed_scaled_color &c)
		  {
			  auto iter=parsed_colors.find(c.from_name);

			  if (iter == parsed_colors.end())
				  return false;

			  static std::optional<double>
				  ui::parsed_scaled_color::*
				  const fields[4]=
				  {
				   &ui::parsed_scaled_color::r,
				   &ui::parsed_scaled_color::g,
				   &ui::parsed_scaled_color::b,
				   &ui::parsed_scaled_color::a
				  };

			  if (!std::holds_alternative<rgb>(iter->second))
			  {
				  // Must not specify any actual scaling.
				  // This is done as a means of copying
				  // the color, only.
				  for (size_t i=0; i<4; ++i)
					  if (c.*(fields[i]))
						  throw EXCEPTION
							  (_("scaled color must"
							     " be an"
							     " rgb color"));

				  parsed_colors.insert_or_assign(id, iter->second);
				  return true;
			  }

			  auto v=std::get<rgb>(iter->second);

			  for (size_t i=0; i<4; ++i)
			  {
				  auto &s=c.*(fields[i]);

				  if (!s)
					  continue;

				  auto sv=*s * v.*(rgb_fields[i]);
				  if (sv > rgb::maximum)
					  sv=rgb::maximum;
				  v.*(rgb_fields[i])=sv;
			  }
			  parsed_colors.insert_or_assign(id, v);
			  return true;
		  },
		  [&]
		  (const ui::parse_linear_gradient &c)
		  {
			  theme_color_t v{std::in_place_type_t<linear_gradient>
					  {}};

			  auto &lg=std::get<linear_gradient>(v);

			  bool found;

			  c.parse(lock, lg,
				  parse_gradient_values{lg.gradient,
								parsed_colors,
								found});

			  if (!found)
				  return false;

			  valid_gradient(lg.gradient);

			  parsed_colors.insert_or_assign(id, std::move(v));
			  return true;
		  },
		  [&]
		  (const ui::parse_radial_gradient &c)
		  {
			  theme_color_t v{std::in_place_type_t<radial_gradient>
					  {}};

			  auto &rg=std::get<radial_gradient>(v);

			  bool found;

			  c.parse(lock, rg,
				  parse_gradient_values{rg.gradient,
								parsed_colors,
								found});
			  if (!found)
				  return false;

			  valid_gradient(rg.gradient);

			  parsed_colors.insert_or_assign(id, std::move(v));
			  return true;
		  }}, parsed_color);
}

namespace ui {
#if 0
}
#endif

void parse_border::parse(const ui::parser_lock &lock)
{
	auto from_attr=lock->get_any_attribute("from");

	if (!from_attr.empty())
		from(from_attr);

	std::string color1_value{parse_color(lock, "color")};

	std::string color2_value;

	if (!color1_value.empty())
	{
		color2_value=parse_color(lock, "color2");
		color(color1_value, color2_value);
	}

	// Parse border dimensions

	static const struct {
		const char *value_field;
		const char *scale_field;
		void (parse_border::*value_callback)(std::string &);
		void (parse_border::*scale_callback)(unsigned);
	} border_dims[]=
		  {
		   {"width", "width_scale",
		    &parse_border::width,
		    &parse_border::width_scale
		   },
		   {"height", "height_scale",
		    &parse_border::height,
		    &parse_border::height_scale
		   },
		   {"hradius", "hradius_scale",
		    &parse_border::hradius,
		    &parse_border::hradius_scale
		   },
		   {"vradius", "vradius_scale",
		    &parse_border::vradius,
		    &parse_border::vradius_scale
		   },
		  };

	for (const auto &d:border_dims)
	{
		if (single_value_exists(lock, d.value_field))
		{
			auto t=single_value(lock, d.value_field, "border");
			(this->*(d.value_callback))(t);
		}

		if (single_value_exists(lock, d.scale_field))
		{
			auto t=single_value(lock, d.scale_field, "border");

			std::istringstream i{t};

			imbue im{lock.c_locale, i};

			unsigned scale;

			// The contents of this node must be a scaling factor.
			i >> scale;

			if (i.fail())
				throw EXCEPTION(_("Cannot parse width_scale"));

			(this->*(d.scale_callback))(scale);
		}
	}

	// <rounded> sets the radii both to 1.

	if (single_value_exists(lock, "rounded"))
	{
		rounded(single_value(lock, "rounded", "border") != "0");
	}

	// Alternatively, hradius and vradius will set them
	// to at least 2.

	auto dash_nodes=lock.clone();

	auto xpath=dash_nodes->get_xpath("dash");

	size_t n=xpath->count();

	std::vector<double> dash_values;

	if (n)
	{
		dash_values.clear();
		dash_values.reserve(n);

		for (size_t i=0; i<n; ++i)
		{
			xpath->to_node(i+1);

			dim_t mm;

			auto t=dash_nodes->get_text();

			if (t.empty())
				continue;

			std::istringstream is{t};

			imbue i_parse{dash_nodes.c_locale, is};

			double v;

			is >> v;

			if (is.fail())
				throw EXCEPTION(_("Cannot parse dash "
						  "values"));

			dash_values.push_back(v);
		}

		dashes(dash_values);
	}
}

std::string parse_border::parse_color(const ui::parser_lock &lock,
				      const char *xpath_name)
{
	std::string s;

	auto color_node=lock.clone();

	auto xpath=color_node->get_xpath(xpath_name);

	if (xpath->count())
	{
		xpath->to_node();

		s=color_node->get_text();
	}
	return s;
}
#if 0
{
#endif
}

namespace {
#if 0
}
#endif

//! Implement border parsing for loaded theme file.
struct parse_theme_border : ui::parse_border {

	//! \<border> being parsed
	const ui::parser_lock &lock;

	//! Loaded generators
	const uigenerators generators;

	//! Border ID
	const std::string id;

	//! flag
	const bool allowthemerefs;

	//! from was not currently found.
	bool notfound=false;

	//! Newly-parsed border.
	border_infomm new_border;

	//! This is a new border. from() resets it to false.
	bool created_border=true;

	//! Constructor
	parse_theme_border(const ui::parser_lock &lock,
			   const uigenerators &generators,
			   const std::string &id,
			   bool allowthemerefs)
		: lock{lock}, generators{generators},
		  id{id}, allowthemerefs{allowthemerefs}
	{
		parse(lock);
	}

private:

	//! Callback
	void from(std::string &) override;

	//! Callback
	void color(std::string &,
		   std::string &) override;

	//! Helper used by dim callbacks.
	void save_dim_arg(std::string &,
			  dim_arg border_infomm::*,
			  const char *name);
	//! Callback
	void width(std::string &) override;
	//! Callback
	void width_scale(unsigned) override;
	//! Callback
	void height(std::string &) override;
	//! Callback
	void height_scale(unsigned) override;

	//! Callback
	void hradius(std::string &) override;
	//! Callback
	void hradius_scale(unsigned) override;

	//! Callback
	void vradius(std::string &) override;
	//! Callback
	void vradius_scale(unsigned) override;

	//! Callback
	void rounded(bool) override;
	//! Callback
	void dashes(std::vector<double> &) override;
};

void parse_theme_border::from(std::string &from_attr)
{
	auto iter=generators->borders.find(from_attr);

	if (iter == generators->borders.end())
	{
		notfound=true;
		return;
	}

	new_border=iter->second;
	created_border=false;
}

void parse_theme_border::color(std::string &color1,
			       std::string &color2)
{
	// If we copied the border from another from, then
	// unless the following values are given, don't
	// touch the colors.

	if (color1.empty())
	{
		if (created_border)
		{
			throw EXCEPTION(gettextmsg
					(_("<color> not specified for "
					   "%1"), id));
		}
		return;
	}
	new_border.color1=generators->lookup_color(color1,
						   allowthemerefs,
						   "color");
	if (!color2.empty())
		new_border.color2=generators->lookup_color(color2,
							   allowthemerefs,
							   "color2");
}

void parse_theme_border::save_dim_arg(std::string &d,
				      dim_arg border_infomm::*arg,
				      const char *name)
{
	std::istringstream i{d};

	imbue im{lock.c_locale, i};

	double v;

	i >> v;

	if (i.fail())
	{
		new_border.*arg=generators->lookup_dim(d, allowthemerefs, name);
	}
	else
	{
		new_border.*arg=v;
	}
}

void parse_theme_border::width(std::string &d)
{
	save_dim_arg(d, &border_infomm::width, "width");
}

void parse_theme_border::width_scale(unsigned s)
{
	new_border.width_scale=s;
}

void parse_theme_border::height(std::string &d)
{
	save_dim_arg(d, &border_infomm::height, "height");
}

void parse_theme_border::height_scale(unsigned s)
{
	new_border.height_scale=s;
}

void parse_theme_border::hradius(std::string &d)
{
	save_dim_arg(d, &border_infomm::hradius, "hradius");
}

void parse_theme_border::hradius_scale(unsigned s)
{
	new_border.hradius_scale=s;
}

void parse_theme_border::vradius(std::string &d)
{
	save_dim_arg(d, &border_infomm::vradius, "vradius");
}

void parse_theme_border::vradius_scale(unsigned s)
{
	new_border.vradius_scale=s;
}

void parse_theme_border::rounded(bool flag)
{
	new_border.rounded=flag;
}

void parse_theme_border::dashes(std::vector<double> &values)
{
	new_border.dashes=std::move(values);
}

#if 0
{
#endif
}

uicompiler::uicompiler(const ui::parser_lock &root_lock,
		       const uigenerators &generators,
		       const const_screen_positionsptr &saved_positions,
		       bool allowthemerefs)
	: generators{generators},
	  saved_positions{saved_positions},
	  allowthemerefs{allowthemerefs}
{
	if (!root_lock->get_root())
		return;

	auto lock=root_lock.clone();

	auto xpath=lock->get_xpath("/theme/color");

	size_t count=xpath->count();

	bool parsed;

	///////////////////////////////////////////////////////////////////////
	//
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

			if (generators->colors.find(id) !=
			    generators->colors.end())
				continue; // Did this one already.

			bool flag;

			try {
				flag=parse_color(lock, id,
						 generators->colors);
			} catch (const exception &e)
			{
				std::ostringstream o;

				o << e;

				throw EXCEPTION(gettextmsg
						(_("Cannot parse color id="
						   "\"%1%\": %2%"),
						 id, o.str()));
			}

			if (flag)
				parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (generators->colors.find(id) != generators->colors.end())
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

	///////////////////////////////////////////////////////////////////////
	//
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

			if (generators->dims.find(id) != generators->dims.end())
				continue; // Did this one already.

			auto value=parse_dim_value(lock,
						   generators->dims, "dim", id);

			if (!value)
				continue;

			generators->dims.insert({id, *value});
			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (generators->dims.find(id) == generators->dims.end())
			unknown_dim(id);
	}

	lock=root_lock.clone();

	xpath=lock->get_xpath("/theme/border");

	count=xpath->count();

	///////////////////////////////////////////////////////////////////////
	//
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

			if (generators->borders.find(id) !=
			    generators->borders.end())
				continue; // Did this one already.

			try {
				parse_theme_border
					parser{lock, generators, id,
					       allowthemerefs};

				if (parser.notfound)
					continue;

				generators->borders.insert_or_assign(id,
							    parser.new_border);

			} catch (const exception &e)
			{
				std::ostringstream o;

				o << e;
				throw EXCEPTION(gettextmsg
						(_("Cannot parse border id="
						   "\"%1%\": %2%"),
						 id, o.str()));
			}

			parsed=true;
		}
	} while (parsed);

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (generators->borders.find(id) != generators->borders.end())
			continue;

		throw EXCEPTION(gettextmsg
				(_("Border %1% is based on another"
				   " border which was not found"
				   " (this can be because of a circular"
				   " reference)"),
				 id));
	}

	load_fonts(root_lock.clone(),
		   [&]
		   (const std::string &id, const font &new_font)
		   {
			   generators->fonts.insert_or_assign(id, new_font);
		   },
		   [&]
		   (const std::string &from) -> std::optional<font>
		   {
			   auto iter=generators->fonts.find(from);

			   if (iter == generators->fonts.end())
				   return std::nullopt;

			   return iter->second;
		   });

	///////////////////////////////////////////////////////////////////////
	//
	// Build a list of uncompiled appearances, by id

	xpath=lock->get_xpath("/theme/appearance");

	count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("Missing <appearance> "
					  "\"id\" element"));

		uncompiled_appearances.insert_or_assign(id, lock->clone());
	}

	// Compile the appearances
	while (!uncompiled_appearances.empty())
	{
		auto name=uncompiled_appearances.begin()->first;

		compile_uncompiled_appearance(name);
	}

	///////////////////////////////////////////////////////////////////////
	//
	// Compile all tooltips

	xpath=lock->get_xpath("/theme/tooltip");

	count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("Missing <tooltip> \"id\""));

		if (generators->tooltip_generators.find(id) !=
		    generators->tooltip_generators.end())
			continue;

		auto tooltip_info=get_label_parameters(lock);

		const auto &[text, config]=tooltip_info;

		generators->tooltip_generators.insert_or_assign
			(id, create_label_tooltip(text, config));
	}

	///////////////////////////////////////////////////////////////////////
	//
	// Build the list of uncompiled_elements, by id.

	xpath=lock->get_xpath("/theme/layout | /theme/factory");

	count=xpath->count();

	for (size_t i=0; i<count; ++i)
	{
		xpath->to_node(i+1);

		auto id=lock->get_any_attribute("id");

		if (id.empty())
			throw EXCEPTION(_("Missing <layout> or <factory>"
					  " \"id\""));

		uncompiled_elements.insert_or_assign(id, lock->clone());
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
				generators->gridlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "menubar")
			{
				auto ret=menubarlayout_parseconfig(lock);
				generators->menubarlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "list")
			{
				auto ret=listlayout_parseconfig(lock);
				generators->listlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "standard_combobox")
			{
				auto ret=standard_comboboxlayout_parseconfig
					(lock);
				generators->standard_comboboxlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "editable_combobox")
			{
				auto ret=editable_comboboxlayout_parseconfig
					(lock);
				generators->editable_comboboxlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "book")
			{
				auto ret=booklayout_parseconfig(lock);
				generators->booklayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "table")
			{
				auto ret=tablelayout_parseconfig(lock);
				generators->tablelayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "pane")
			{
				auto ret=panelayout_parseconfig(lock);
				generators->panelayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "item")
			{
				auto ret=itemlayout_parseconfig(lock);
				generators->itemlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "page")
			{
				auto ret=pagelayout_parseconfig(lock);
				generators->pagelayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "toolbox")
			{
				auto ret=toolboxlayout_parseconfig(lock);
				generators->toolboxlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "border")
			{
				auto ret=borderlayout_parseconfig(lock);
				generators->borderlayoutmanager_generators
					.insert_or_assign(id, ret);
				continue;
			}
		}
		else if (name == "factory")
		{
			if (type == "factory")
			{
				auto ret=factory_parseconfig(lock);
				generators->factory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "grid")
			{
				auto ret=gridfactory_parseconfig(lock);
				generators->gridfactory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "menubar")
			{
				auto ret=menubarfactory_parseconfig(lock);
				generators->menubarfactory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "bookpage")
			{
				auto ret=bookpagefactory_parseconfig(lock);
				generators->bookpagefactory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "page")
			{
				auto ret=pagefactory_parseconfig(lock);
				generators->pagefactory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "pane")
			{
				auto ret=panefactory_parseconfig(lock);
				generators->panefactory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "toolbox")
			{
				auto ret=toolboxfactory_parseconfig(lock);
				generators->toolboxfactory_generators
					.insert_or_assign(id, ret);
				continue;
			}

			if (type == "elements")
			{
				auto ret=elements_parseconfig(lock);

				generators->elements_generators
					.insert_or_assign(id, ret);
				continue;
			}
		}

		throw EXCEPTION(gettextmsg(_("Unrecognized %1% type \"%2%\""),
					   name,
					   type));
	}
}

namespace {
#if 0
}
#endif

struct parse_font : ui::parse_font {

	font &f;

	parse_font( font &f) : f{f} {}

	void set_point_size(double v) override
	{
		f.set_point_size(v);
	}

	void set_scaled_size(double v) override
	{
		f.set_scaled_size(v);
	}

	void scale(double v) override
	{
		f.scale(v);
	}

	void set_from(const std::string &v) override
	{
		// Already processed ourselves
	}

	void set_family(const std::string &v) override
	{
		f.set_family(v);
	}

	void set_foundry(const std::string &v) override
	{
		f.set_foundry(v);
	}

	void set_style(const std::string &v) override
	{
		f.set_style(v);
	}

	void set_weight(const std::string &v) override
	{
		f.set_weight(v);
	}

	void set_spacing(const std::string &v) override
	{
		f.set_spacing(v);
	}

	void set_slant(const std::string &v) override
	{
		f.set_slant(v);
	}

	void set_width(const std::string &v) override
	{
		f.set_width(v);
	}
};

static const struct {

	//! Name of an element inside a <font>
	const char *name;

	//! font method that sets the value.

	std::variant<
		void (ui::parse_font::*)(double),
		void (ui::parse_font::*)(const std::string &)
		> set_font_value;
} fontfields[]=
	{
	 { "point_size", &ui::parse_font::set_point_size},
	 { "scaled_size", &ui::parse_font::set_scaled_size},
	 { "scale", &ui::parse_font::scale},

	 { "from", &ui::parse_font::set_from},
	 { "family", &ui::parse_font::set_family},
	 { "foundry", &ui::parse_font::set_foundry},
	 { "style", &ui::parse_font::set_style},
	 { "weight", &ui::parse_font::set_weight},
	 { "spacing", &ui::parse_font::set_spacing},
	 { "slant", &ui::parse_font::set_slant},
	 { "width", &ui::parse_font::set_width},
	};


#if 0
{
#endif
}

namespace ui {
#if 0
}
#endif

// Parse what's inside a <font>

void parse_font::parse(const ui::parser_lock &lock,
		       const std::string &id)
{
	bool size_field_found=false;

	for (const auto &v:fontfields)
	{
		auto node=lock.clone();

		auto xpath=node->get_xpath(v.name);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		auto s=node->get_text();

		std::visit
			(visitor
			 {[&](void (ui::parse_font::*func)(double))
			  {
				  try {
					  if (size_field_found)
						  throw EXCEPTION
							  (_("Multiple size"
							     " fields"
							     " specified"));

					  auto v=x::value_string<double>
						  ::from_string
						  (s, lock.c_locale);

					  (this->*func)(v);
				  } catch (const x::exception &e)
				  {
					  std::ostringstream o;

					  o << gettextmsg(_("Cannot parse %1%, "
							    " font id=%2%: "),
							  v.name,
							  id);
					  o << e;
					  throw EXCEPTION(o.str());
				  }
				  size_field_found=true;

			  },[&](void (ui::parse_font::*string_func)
				(const std::string &))
			    {
				    (this->*string_func)(s);
			    }
			 },
			 v.set_font_value);
	}
}

#if 0
{
#endif
}

void uicompiler::do_load_fonts(const ui::parser_lock &lock,
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

			auto from=optional_value(lock, "from", "font");

			if (!from.empty())
			{
				auto ret=lookup(from);

				if (!ret)
					// Not yet parsed
					continue;
				new_font=*ret;
			}

			{
				parse_font parser{new_font};

				parser.parse(lock, id);
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

const_vector<gridlayoutmanager_generator>
uicompiler::lookup_gridlayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->gridlayoutmanager_generators.find(name);

		if (iter != generators->gridlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "grid");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=gridlayout_parseconfig(new_lock);

	generators->gridlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<listlayoutmanager_generator>
uicompiler::lookup_listlayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->listlayoutmanager_generators.find(name);

		if (iter != generators->listlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "list");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=listlayout_parseconfig(new_lock);

	generators->listlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<standard_comboboxlayoutmanager_generator>
uicompiler::lookup_standard_comboboxlayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->standard_comboboxlayoutmanager_generators.find(name);

		if (iter != generators->standard_comboboxlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "standard_combobox");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=standard_comboboxlayout_parseconfig(new_lock);

	generators->standard_comboboxlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<editable_comboboxlayoutmanager_generator>
uicompiler::lookup_editable_comboboxlayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->editable_comboboxlayoutmanager_generators.find(name);

		if (iter != generators->editable_comboboxlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "editable_combobox");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=editable_comboboxlayout_parseconfig(new_lock);

	generators->editable_comboboxlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<tablelayoutmanager_generator>
uicompiler::lookup_tablelayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->tablelayoutmanager_generators.find(name);

		if (iter != generators->tablelayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "table");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=tablelayout_parseconfig(new_lock);

	generators->tablelayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<panelayoutmanager_generator>
uicompiler::lookup_panelayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->panelayoutmanager_generators.find(name);

		if (iter != generators->panelayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "pane");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=panelayout_parseconfig(new_lock);

	generators->panelayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<panefactory_generator>
uicompiler::lookup_panefactory_generators(const ui::parser_lock &lock,
					  const char *element,
					  const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->panefactory_generators.find(name);

		if (iter != generators->panefactory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "pane");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=panefactory_parseconfig(new_lock);

	generators->panefactory_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<toolboxfactory_generator>
uicompiler::lookup_toolboxfactory_generators(const ui::parser_lock &lock,
					     const char *element,
					     const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->toolboxfactory_generators.find(name);

		if (iter != generators->toolboxfactory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "toolbox");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=toolboxfactory_parseconfig(new_lock);

	generators->toolboxfactory_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<itemlayoutmanager_generator>
uicompiler::lookup_itemlayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->itemlayoutmanager_generators.find(name);

		if (iter != generators->itemlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "item");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=itemlayout_parseconfig(new_lock);

	generators->itemlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<pagelayoutmanager_generator>
uicompiler::lookup_pagelayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->pagelayoutmanager_generators.find(name);

		if (iter != generators->pagelayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "page");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=pagelayout_parseconfig(new_lock);

	generators->pagelayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<toolboxlayoutmanager_generator>
uicompiler::lookup_toolboxlayoutmanager_generators(const ui::parser_lock &lock,
						   const std::string &name)
{
	{
		auto iter=generators->toolboxlayoutmanager_generators.find(name);

		if (iter != generators->toolboxlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "toolbox");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=toolboxlayout_parseconfig(new_lock);

	generators->toolboxlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<booklayoutmanager_generator>
uicompiler::lookup_booklayoutmanager_generators(const ui::parser_lock &lock,
						const std::string &name)
{
	{
		auto iter=generators->booklayoutmanager_generators.find(name);

		if (iter != generators->booklayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "book");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=booklayout_parseconfig(new_lock);

	generators->booklayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<gridfactory_generator>
uicompiler::lookup_gridfactory_generators(const ui::parser_lock &lock,
					  const char *element,
					  const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->gridfactory_generators.find(name);

		if (iter != generators->gridfactory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "grid");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=gridfactory_parseconfig(new_lock);

	generators->gridfactory_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<factory_generator>
uicompiler::lookup_factory_generators(const ui::parser_lock &lock,
				      const char *element,
				      const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->factory_generators.find(name);

		if (iter != generators->factory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "factory");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=factory_parseconfig(new_lock);

	generators->factory_generators.insert_or_assign(name, ret);

	return ret;
}


const_vector<menubarlayoutmanager_generator>
uicompiler::lookup_menubarlayoutmanager_generators
(const ui::parser_lock &lock,
 const char *element,
 const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->menubarlayoutmanager_generators
			.find(name);

		if (iter != generators->menubarlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "menubar");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=menubarlayout_parseconfig(new_lock);

	generators->menubarlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<menubarfactory_generator>
uicompiler::lookup_menubarfactory_generators(const ui::parser_lock &lock,
					     const char *element,
					     const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->menubarfactory_generators.find(name);

		if (iter != generators->menubarfactory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "menubar");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=menubarfactory_parseconfig(new_lock);

	generators->menubarfactory_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<pagefactory_generator>
uicompiler::lookup_pagefactory_generators(const ui::parser_lock &lock,
					      const char *element,
					      const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->pagefactory_generators.find(name);

		if (iter != generators->pagefactory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "page");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=pagefactory_parseconfig(new_lock);

	generators->pagefactory_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<bookpagefactory_generator>
uicompiler::lookup_bookpagefactory_generators(const ui::parser_lock &lock,
					      const char *element,
					      const char *parent)
{
	auto name=single_value(lock, element, parent);

	{
		auto iter=generators->bookpagefactory_generators.find(name);

		if (iter != generators->bookpagefactory_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "factory", "bookpage");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=bookpagefactory_parseconfig(new_lock);

	generators->bookpagefactory_generators.insert_or_assign(name, ret);

	return ret;
}

const_vector<borderlayoutmanager_generator>
uicompiler::lookup_borderlayoutmanager_generators(const ui::parser_lock &lock,
						  const std::string &name)
{
	{
		auto iter=generators->borderlayoutmanager_generators.find(name);

		if (iter != generators->borderlayoutmanager_generators.end())
			return iter->second;
	}

	auto iter=find_uncompiled(name, "layout", "border");

	auto new_lock=iter->second;

	uncompiled_elements.erase(iter);

	auto ret=borderlayout_parseconfig(new_lock);

	generators->borderlayoutmanager_generators.insert_or_assign(name, ret);

	return ret;
}

void uicompiler::singletonlayout_replace(const singletonlayoutmanager &layout,
					 uielements &factories,
					 const const_vector
					 <factory_generator> &generators)
{
	auto f=layout->replace();

	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler::gridlayout_append_row(const gridlayoutmanager &layout,
				       uielements &factories,
				       const const_vector<gridfactory_generator>
				       &g)
{
	generate_gridfactory(layout->append_row(), factories, g);
}

void uicompiler::gridlayout_insert_row(const gridlayoutmanager &layout,
				       size_t row,
				       uielements &factories,
				       const const_vector<gridfactory_generator>
				       &g)
{
	generate_gridfactory(layout->insert_row(row), factories, g);
}

void uicompiler::gridlayout_replace_row(const gridlayoutmanager &layout,
					size_t row,
					uielements &factories,
					const const_vector
					<gridfactory_generator> &g)
{
	generate_gridfactory(layout->replace_row(row), factories, g);
}

void uicompiler::gridlayout_append_columns(const gridlayoutmanager &layout,
					   size_t row,
					   uielements &factories,
					   const
					   const_vector<gridfactory_generator>
					   &g)
{
	generate_gridfactory(layout->append_columns(row), factories, g);
}

void uicompiler::gridlayout_insert_columns(const gridlayoutmanager &layout,
					   size_t row,
					   size_t col,
					   uielements &factories,
					   const const_vector
					   <gridfactory_generator> &g)
{
	generate_gridfactory(layout->insert_columns(row, col), factories, g);
}

void uicompiler::gridlayout_replace_cell(const gridlayoutmanager &layout,
					 size_t row,
					 size_t col,
					 uielements &factories,
					 const
					 const_vector<gridfactory_generator> &g)
{
	generate_gridfactory(layout->replace_cell(row, col), factories, g);
}

void uicompiler::generate_gridfactory(const gridfactory &f,
				      uielements &factories,
				      const const_vector<gridfactory_generator>
				      &generators)
{
	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

void uicompiler::booklayout_append_pages(const booklayoutmanager &blm,
					 uielements &factories,
					 const
					 const_vector<bookpagefactory_generator>
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
					 const
					 const_vector<bookpagefactory_generator>
					 &generators)
{
	auto f=blm->insert(pos);

	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

factory_generator uicompiler::factory_parseconfig(const ui::parser_lock &lock,
						  const char *element,
						  const char *parent)
{
	auto element_lock=lock->clone();
	element_lock->get_xpath(element)->to_node();

	return [generators=factory_parseconfig(element_lock)]
		(const factory &f, uielements &elements)
	       {
		       for (const auto &generator:*generators)
			       generator(f, elements);
	       };
}

// Additional helpers used by appearance_parser

color_arg uicompiler::to_color_arg(const ui::parser_lock &lock,
				   const char *element, const char *parent)
{
	return generators->lookup_color(single_value(lock, ".", parent),
				       allowthemerefs, element);
}

border_arg uicompiler::to_border_arg(const ui::parser_lock &lock,
				     const char *element, const char *parent)
{
	return generators->lookup_border(single_value(lock, ".", parent),
					allowthemerefs, element);
}

dim_arg uicompiler::to_dim_arg(const ui::parser_lock &lock,
			       const char *element, const char *parent)
{
	return generators->lookup_dim(single_value(lock, ".", parent),
				     allowthemerefs, element);
}

font_arg uicompiler::to_font_arg(const ui::parser_lock &lock,
				 const char *element, const char *parent)
{
	return generators->lookup_font(single_value(lock, ".", parent),
				      allowthemerefs, element);
}

static void cannot_convert_color_arg(const char *element, const char *parent)
	__attribute__((noreturn));

void cannot_convert_color_arg(const char *element, const char *parent)
{
	throw EXCEPTION(gettextmsg
			(_("Color <%1%> (%2%) cannot be a text color"),
			 element, parent));
}

text_color_arg uicompiler::to_text_color_arg(const ui::parser_lock &lock,
					     const char *element,
					     const char *parent)
{
	return to_text_color_arg(to_color_arg(lock, element, parent),
				 element,
				 parent);
}

text_color_arg uicompiler::to_text_color_arg(const color_arg &color,
					     const char *element,
					     const char *parent)
{
	return std::visit(visitor{
			[&](const std::string &s) -> text_color_arg
			{
				return theme_color{s};
			},
			[&](const auto &v) -> text_color_arg
			{
				typedef std::remove_cv_t
					<std::remove_reference_t<decltype(v)>>
					v_t;

				if constexpr(visited_v<v_t, text_color_arg>)
				{
					return v;
				}
				else
				{
					cannot_convert_color_arg(element,
								 parent);
				}
			}}, color);
}

text_param uicompiler::text_param_value(const ui::parser_lock &lock,
					const char *element,
					const char *parent)
{
	text_param t;

	t(theme_text{single_value(lock, element, parent), generators});

	return t;
}

shortcut uicompiler::shortcut_value(const ui::parser_lock &lock,
				    const char *element,
				    const char *parent)
{
	return optional_value(lock, element, parent);
}

rgb uicompiler::rgb_value(const ui::parser_lock &lock,
			   const char *element, const char *parent)
{
	auto c=generators->lookup_color(single_value(lock, element, parent),
					allowthemerefs, element);

	if (!std::holds_alternative<rgb>(c))
		throw EXCEPTION(gettextmsg("<%1%> in <%2%> must specify an rgb"
					   " color", element, parent));

	return std::get<rgb>(c);
}

font uicompiler::font_value(const ui::parser_lock &lock,
			    const char *element, const char *parent)
{
	auto c=generators->lookup_font(single_value(lock, element, parent),
				       allowthemerefs, element);

	if (!std::holds_alternative<font>(c))
		throw EXCEPTION(gettextmsg("<%1%> in <%2%> must specify a"
					   " font", element, parent));

	return std::get<font>(c);
}


void uicompiler::compile_uncompiled_appearance(const std::string &name)
{
	auto iter=uncompiled_appearances.find(name);

	if (iter == uncompiled_appearances.end())
		return;

	auto lock=iter->second;

	uncompiled_appearances.erase(iter);

	generators->loaded_appearances.insert_or_assign
		(name,
		 compile_appearance(lock,
				    lock->get_any_attribute("type")));
}

void uicompiler::menubarlayout_append_menus(const menubarlayoutmanager &layout,
					    uielements &factories,
					    const const_vector
					    <menubarfactory_generator> &g)
{
	generate_menubarfactory(layout->append_menus(), factories, g);
}

void uicompiler::menubarlayout_append_right_menus
(const menubarlayoutmanager &layout,
 uielements &factories,
 const const_vector <menubarfactory_generator> &g)
{
	generate_menubarfactory(layout->append_right_menus(), factories, g);
}


void uicompiler::menubarlayout_insert_menus(const menubarlayoutmanager &layout,
					    uielements &factories,
					    size_t pos,
					    const const_vector
					    <menubarfactory_generator> &g)
{
	generate_menubarfactory(layout->insert_menus(pos), factories, g);
}

void uicompiler::menubarlayout_insert_right_menus
(const menubarlayoutmanager &layout,
 uielements &factories,
 size_t pos,
 const const_vector <menubarfactory_generator> &g)
{
	generate_menubarfactory(layout->insert_right_menus(pos), factories, g);
}

void uicompiler::generate_menubarfactory(const menubarfactory &f,
					 uielements &factories,
					 const const_vector<
					 menubarfactory_generator>
					 &generators)
{
	for (const auto &g:*generators)
	{
		g(f, factories);
	}
}

LIBCXXW_NAMESPACE_END
