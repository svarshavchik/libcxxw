/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/screen_positions.H"
#include "main_window_handler.H"
#include "screen.H"
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

screen_positions::screen_positions() : data{xml::doc::create()}
{
}

screen_positions::~screen_positions()=default;

screen_positions::screen_positions(screen_positions &&)=default;


void screen_positions::save(const std::string &filename) const
{
	auto lock=data->readlock();

	lock->save_file(filename);
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::load_screen_positions, load_log);

static auto load(const std::string &filename)
{
	LOG_FUNC_SCOPE(load_log);

	if (access(filename.c_str(), R_OK) == 0)
	{
		try
		{
			try {
				return xml::doc::create(filename);
			} catch (const exception &e)
			{
				throw EXCEPTION(filename << ": " << e);
			}
		} CATCH_EXCEPTIONS;
	}

	return xml::doc::create();
}

screen_positions::screen_positions(const std::string &filename)
	: data{load(filename)}
{
}

static std::string window_name_to_xpath(const std::string &window_name)
{
	return "/windows/window[name="
		+ xml::quote_string_literal(window_name) + "]";
}

void main_windowObj::save(const std::string &window_name,
			  screen_positions &pos) const
{
	auto handler=impl->handler;

	auto [wx, wy] = handler->root_xy.get();

	auto r=handler->current_position.get();

	auto lock=pos.data->writelock();

	if (lock->get_root())
	{
		auto xpath=lock->get_xpath(window_name_to_xpath(window_name));

		size_t n=xpath->count();

		for (size_t i=1; i <= n; ++i)
		{
			xpath->to_node(i);
			lock->remove();
		}
	}
	else
	{
		lock->create_child()->element({"windows"});
		lock->get_root();
	}

	auto xpath=lock->get_xpath("/windows");

	if (xpath->count() <= 0)
	{
		lock->remove();
		lock->create_child()->element({"windows"});
	}
	else
	{
		xpath->to_node();
	}

	auto window=lock->create_child()->element({"window"})
		->element({"name"})->text(window_name);

	std::ostringstream x, y, width, height;

	x << wx;
	y << wy;
	width << r.width;
	height << r.height;

	if (preserve_screen_number_prop.get())
	{
		std::ostringstream screen_number;

		screen_number << get_screen()->impl->screen_number;

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


std::optional<screen_positions::window_info>
screen_positions::find(const std::string &window_name) const
{
	LOG_FUNC_SCOPE(load_log);

	auto lock=data->readlock();

	std::optional<window_info> info;

	if (!lock->get_root())
		return info;

	auto xpath=lock->get_xpath(window_name_to_xpath(window_name));

	size_t n=xpath->count();

	if (n == 1)
	{
		xpath->to_node();

		try {
			info={lock};

			auto value=lock->clone();

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

			auto &r=info->coordinates;

			std::istringstream{x} >> r.x;
			std::istringstream{y} >> r.y;
			std::istringstream{width} >> r.width;
			std::istringstream{height} >> r.height;

			value=lock->clone();

			auto xpath=value->get_xpath("screen");

			if (xpath->count() == 1 &&
			    preserve_screen_number_prop.get())
			{
				xpath->to_node();

				size_t n=0;
				std::istringstream{value->get_text()} >> n;
				info->screen_number=n;
			}

			return info;
		} CATCH_EXCEPTIONS;

		info.reset();
	}

	return info;
}

void preserve_screen_number(bool flag)
{
	property::load_property(LIBCXX_NAMESPACE_STR
				"::w::preserve_screen_number",
				flag ? "true":"false", true, true);
}

LIBCXXW_NAMESPACE_END
