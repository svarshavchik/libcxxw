/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/main_windowfwd.H"
#include "catch_exceptions.H"
#include <x/xml/doc.H>
#include <x/exception.H>
#include <x/logger.H>
#include <x/property_value.H>

#include <unistd.h>
#include <sstream>

LIBCXXW_NAMESPACE_START

static property::value<bool>
preserve_screen_number_prop(LIBCXX_NAMESPACE_STR "::w::preserve_screen_number",
			    true);

void save_screen_positions(const std::string &filename,
			   const screen_positions_t &coordinates)
{
	auto doc=xml::doc::create();

	auto lock=doc->writelock();

	auto windows=lock->create_child()->element({"windows"});

	for (const auto &coords:coordinates)
	{
		// Position the lock:

		lock->get_xpath("/windows")->to_node();

		auto window=lock->create_child()->element({"window"})
			->element({"name"})->text(coords.first);

		std::ostringstream x, y, width, height;

		x << coords.second.x;
		y << coords.second.y;
		width << coords.second.width;
		height << coords.second.height;

		if (coords.second.screen_number &&
		    preserve_screen_number_prop.get())
		{
			std::ostringstream screen_number;

			screen_number << *coords.second.screen_number;
			window=window->parent()->create_next_sibling()
				->element({"screen"})
				->create_child()->text(screen_number.str());
		}
		window=window->parent()->create_next_sibling()->element({"x"})
			->create_child()->text(x.str());
		window=window->parent()->create_next_sibling()->element({"y"})
			->create_child()->text(y.str());
		window=window->parent()->create_next_sibling()
			->element({"width"})->create_child()->text(width.str());
		window->parent()->create_next_sibling()->element({"height"})
			->create_child()->text(height.str());
	}

	lock->save_file(filename);
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::load_screen_positions, load_log);

static void load_screen_positions(const std::string &filename,
				  screen_positions_t &pos)
{
	LOG_FUNC_SCOPE(load_log);

	LOG_DEBUG("Loading " << filename);
	auto doc=xml::doc::create(filename);

	auto lock=doc->readlock();

	lock->get_root();
	auto windows=lock->get_xpath("/windows/window");

	size_t n=windows->count();

	LOG_DEBUG("Found " << n << " saved positions");

	for (size_t i=1; i<=n; ++i)
	{
		windows->to_node(i);

		try {
			auto value=lock->clone();

			value->get_xpath("name")->to_node();

			auto name=value->get_text();

			LOG_DEBUG("Loading " << name);

			value=lock->clone();

			value->get_xpath("x")->to_node();

			auto x=value->get_text();

			value=lock->clone();

			value->get_xpath("y")->to_node();

			auto y=value->get_text();

			value=lock->clone();

			value->get_xpath("width")->to_node();

			auto width=value->get_text();

			value=lock->clone();

			value->get_xpath("height")->to_node();

			auto height=value->get_text();

			rectangle r;

			std::istringstream{x} >> r.x;
			std::istringstream{y} >> r.y;
			std::istringstream{width} >> r.width;
			std::istringstream{height} >> r.height;

			std::optional<size_t> screen_number;

			value=lock->clone();

			auto xpath=value->get_xpath("screen_number");

			if (xpath->count() == 1 &&
			    preserve_screen_number_prop.get())
			{
				xpath->to_node();

				size_t n=0;
				std::istringstream{value->get_text()} >> n;
				screen_number=n;
			}

			pos.emplace(name, screen_position{r, screen_number});

		} catch (const exception &e)
		{
			LOG_ERROR(filename << ": " << e);
			continue;
		}
	}
}

screen_positions_t load_screen_positions(const std::string &filename)
{
	LOG_FUNC_SCOPE(load_log);

	screen_positions_t pos;

	try {
		try {
			if (access(filename.c_str(), R_OK) == 0)
				load_screen_positions(filename, pos);
		} catch (const exception &e)
		{
			throw EXCEPTION(filename << ": " << e);
		}
	} CATCH_EXCEPTIONS;

	return pos;
}

void preserve_screen_number(bool flag)
{
	property::load_property(LIBCXX_NAMESPACE_STR
				"::w::preserve_screen_number",
				flag ? "true":"false", true, true);
}

LIBCXXW_NAMESPACE_END