/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "x/w/impl/uixmlparser.H"
#include "messages.H"
#include "defaulttheme.H"
#include <x/imbue.H>
#include <x/xml/xpath.H>
#include <sstream>
#include <charconv>

LIBCXXW_NAMESPACE_START

namespace ui {
#if 0
}
#endif

parse_color_t parse_color(const parser_lock &lock)
{
	parse_color_t parsed_color;

	auto type=lock->get_any_attribute("type");

	if (type == "linear_gradient")
	{
		parsed_color.emplace<parse_linear_gradient>();

		return parsed_color;
	}

	if (type == "radial_gradient")
	{
		parsed_color.emplace<parse_radial_gradient>();

		return parsed_color;
	}

	if (!type.empty() && type != "rgb")
	{
		throw EXCEPTION(gettextmsg(_("Unknown color type=%1%"),
					   type));
	}

	auto scale=lock->get_any_attribute("scale");

	parsed_scaled_color parsed_scaled_color_v;

	auto &scaled=
		scale.empty() ? parsed_scaled_color_v
		: parsed_color.emplace<parsed_scaled_color>();

	scaled.from_name=scale;

	static std::optional<double> parsed_scaled_color::* const fields[4]=
		{
		 &parsed_scaled_color::r,
		 &parsed_scaled_color::g,
		 &parsed_scaled_color::b,
		 &parsed_scaled_color::a
		};

	for (size_t i=0; i<4; ++i)
	{
		auto attribute=lock.clone();

		auto xpath=attribute->get_xpath(rgb_channels[i]);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		auto text=attribute->get_text();

		double v;

		auto ret=std::from_chars(text.c_str(), text.c_str()+
					 text.size(), v);

		if (ret.ec != std::errc{} || *ret.ptr)
			throw EXCEPTION(gettextmsg(_("could not parse \"%1%\""
						     " as a color"),
						   text));

		if (v < 0)
			throw EXCEPTION(_("negative color value"));

		scaled.*(fields[i])=v;
	}

	if (!scale.empty())
		return parsed_color;

	rgb &color=std::get<rgb>(parsed_color);

	for (size_t i=0; i<4; ++i)
	{
		auto &value=scaled.*(fields[i]);

		if (!value)
			continue;

		if (*value > 1)
			value=1;

		color.*(rgb_fields[i])=(*value) * rgb::maximum;
	}

	return parsed_color;
}


void parse_linear_gradient::do_parse(const ui::parser_lock &lock,
				     linear_gradient_values &color,
				     const function<void (size_t,
							  const std::string &)
				     > &cb) const
{
	static const char * const coords[]={"x1",
					    "y1",
					    "x2",
					    "y2",
					    "widthmm",
					    "heightmm"};

	static const double minvalue[]={0,0,0,0,-999,-999};
	static const double maxvalue[]={1,1,1,1,999,999};

	static double linear_gradient_values::* const fields[]={
		&linear_gradient_values::x1,
		&linear_gradient_values::y1,
		&linear_gradient_values::x2,
		&linear_gradient_values::y2,
		&linear_gradient_values::fixed_width,
		&linear_gradient_values::fixed_height};

	for (size_t i=0; i<6; i++)
	{
		auto attribute=lock.clone();

		auto xpath=attribute->get_xpath(coords[i]);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		auto text=attribute->get_text();

		double v;

		auto ret=std::from_chars(text.c_str(), text.c_str()+
					 text.size(), v);

		if (ret.ec != std::errc{} || *ret.ptr)
			throw EXCEPTION(gettextmsg
					(_("could not parse <%1%>"),
					 coords[i]));

		if (v < minvalue[i] || v>maxvalue[i])
			throw EXCEPTION(gettextmsg
					(_("<%1%> must be between %2%"
					   " and %3%"),
					 coords[i],
					 minvalue[i], maxvalue[i]));

		color.*(fields[i])=v;
	}
	parse_gradient_values(lock, cb);
}

void parse_radial_gradient::do_parse(const ui::parser_lock &lock,
				     radial_gradient_values &color,
				     const function<void (size_t,
							  const std::string &)
				     > &cb) const
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

	static double radial_gradient_values::* const fields[]={
		&radial_gradient_values::inner_center_x,
		&radial_gradient_values::inner_center_y,
		&radial_gradient_values::outer_center_x,
		&radial_gradient_values::outer_center_y,
		&radial_gradient_values::inner_radius,
		&radial_gradient_values::outer_radius,
		&radial_gradient_values::fixed_width,
		&radial_gradient_values::fixed_height};

	for (size_t i=0; i<6; i++)
	{
		auto attribute=lock.clone();

		auto xpath=attribute->get_xpath(coords[i]);

		if (xpath->count() == 0)
			continue;

		xpath->to_node();

		auto text=attribute->get_text();

		double v;

		auto ret=std::from_chars(text.c_str(), text.c_str()+
					 text.size(), v);

		if (ret.ec != std::errc{} || *ret.ptr)
			throw EXCEPTION(gettextmsg
					(_("could not parse <%1%>"),
					 coords[i]));

		if (v < minvalue[i] || v>maxvalue[i])
			throw EXCEPTION(gettextmsg
					(_("<%1%> must be between %2%"
					   " and %3%"),
					 coords[i],
					 minvalue[i], maxvalue[i]));

		color.*(fields[i])=v;
	}

	static const char * const raxises[]={"inner_radius_axis",
					     "outer_radius_axis"};

	static radial_gradient_values::radius_axis
		radial_gradient_values::* const rfields[]=
		{
		 &radial_gradient_values::inner_radius_axis,
		 &radial_gradient_values::outer_radius_axis
		};

	for (size_t i=0; i<2; i++)
	{
		if (!single_value_exists(lock, raxises[i]))
			continue;

		auto s=lowercase_single_value(lock, raxises[i], "color");

		if (s.empty())
			continue;

		if (s == "horizontal")
		{
			color.*(rfields[i])=radial_gradient_values::horizontal;
		}
		else if (s == "vertical")
		{
			color.*(rfields[i])=radial_gradient_values::vertical;
		}
		else if (s == "shortest")
		{
			color.*(rfields[i])=radial_gradient_values::shortest;
		}
		else if (s == "longest")
		{
			color.*(rfields[i])=radial_gradient_values::longest;
		}
		else
			throw EXCEPTION(gettextmsg
					(_("%1% is not a valid value for"
					   " <%2%>"),
					 s, raxises[i]));
	}
	parse_gradient_values(lock, cb);
}

void parse_gradient
::parse_gradient_values(const ui::parser_lock &lock,
			const function<void (size_t,
					     const std::string &)
			> &cb) const
{
	auto gradients=lock->clone();

	auto xpath=gradients->get_xpath("gradient");

	size_t n=xpath->count();

	for (size_t i=0; i<n; ++i)
	{
		xpath->to_node(i+1);

		auto gradient=gradients->clone();

		auto vxpath=gradient->get_xpath("value");

		vxpath->to_node();

		auto value=gradient->get_text();

		unsigned n;

		const char *cp=value.c_str();

		auto ret=std::from_chars(cp, cp+value.size(), n);

		if (ret.ec != std::errc() || *ret.ptr)
			throw EXCEPTION("Invalid gradient <value>");

		gradient=gradients->clone();
		vxpath=gradient->get_xpath("color");
		vxpath->to_node();

		cb(n, gradient->get_text());
	}
}

#if 0
{
#endif
}
LIBCXXW_NAMESPACE_END
