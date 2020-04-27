/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_positions_impl.H"
#include "main_window_handler.H"
#include "screen.H"
#include "catch_exceptions.H"
#include "messages.H"
#include "x/w/dialog.H"
#include <x/xml/doc.H>
#include <x/xml/xpath.H>
#include <x/exception.H>
#include <x/logger.H>
#include <x/property_value.H>

#include <unistd.h>
#include <sstream>

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::load_screen_positions, load_log);

static property::value<bool>
preserve_screen_number_prop(LIBCXX_NAMESPACE_STR "::w::preserve_screen_number",
			    true);

screen_positionsObj::implObj::implObj()
	: data{xml::doc::create()}
{
}

screen_positionsObj::implObj::~implObj()=default;


void screen_positionsObj::implObj::save(const std::string &filename) const
{
	auto lock=data->readlock();

	lock->save_file(filename);
}

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

screen_positionsObj::implObj::implObj(const std::string &filename)
	: data{load(filename)}
{
}

std::string saved_element_to_xpath(const std::string_view &type,
				   const std::string_view &name)
{
	std::string s;

	s.reserve(type.size()+name.size()+20);

	s += "/windows/";

	s += type;
	s += "[name=";
	s += xml::quote_string_literal(std::string{name});
	s += "]";

	return s;
}

xml::writelock
screen_positionsObj::implObj
::create_writelock_for_saving(const std::string_view &type,
			      const std::string_view &name_s)
{
	std::string name{name_s};

	auto lock=data->writelock();

	if (lock->get_root())
	{
		auto xpath=lock->get_xpath(saved_element_to_xpath(type,
								  name_s));

		// Remove any existing memorized setting.
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
	}

	lock->get_xpath("/windows")->to_node();

	lock->create_child()->element(std::string{type})
		->element({"name"})->text(name)->parent()->parent();

	return lock;
}

void main_windowObj::save(const screen_positions &pos) const
{
	auto handler=impl->handler;

	if (handler->window_id.empty())
		throw EXCEPTION(_("Window label was not set."));

	in_thread([me=const_ref{this},
		   pos,
		   lock=pos->impl->create_shared()]
		  (ONLY IN_THREAD)
		  {
			  me->save(IN_THREAD, pos);
		  });
}

void main_windowObj::save(ONLY IN_THREAD,
			  const screen_positions &pos) const
{
	auto handler=impl->handler;

	if (handler->window_id.empty())
		throw EXCEPTION(_("Window label was not set."));

	auto [wx, wy] = handler->root_xy.get();

	auto r=handler->current_position.get();

	// A window or a dialog can be created but never shown, so its
	// screen position will not get initialized. Avoid saving this
	// window's coordinates, in this case.

	if (handler->has_exposed(IN_THREAD))
	{
		auto lock=pos->impl
			->create_writelock_for_saving("window",
						      handler->window_id);

		auto window=lock->create_child()->element({"x"})->text(wx);
		window=window->parent()->create_next_sibling()->element({"y"})
			->create_child()->text(wy);
		window=window->parent()->create_next_sibling()
			->element({"width"})->create_child()->text(r.width);
		window->parent()->create_next_sibling()->element({"height"})
			->create_child()->text(r.height);

		if (preserve_screen_number_prop.get())
		{
			std::ostringstream screen_number;

			screen_number << get_screen()->impl->screen_number;

			window=window->parent()->create_next_sibling()
				->element({"screen"})
				->create_child()->text(screen_number.str());
		}
	}

	std::vector<dialog> all_dialogs;

	{
		implObj::all_dialogs_t::lock lock{impl->all_dialogs};

		all_dialogs.reserve(lock->size());

		for (const auto &dialogs:*lock)
			all_dialogs.push_back(dialogs.second);
	}

	// Recursively invoke save() of all containers/elements in the window

	handler->save(IN_THREAD, pos);

	// Recursively save all dialog positions.

	for (const auto &d:all_dialogs)
	{
		auto handler=d->dialog_window->impl->handler;

		if (handler->window_id.empty())
			continue;

		d->dialog_window->save(IN_THREAD, pos);
	}
}


std::optional<main_window_config::window_info_t>
screen_positionsObj::implObj::find(const std::string_view &window_name) const
{
	LOG_FUNC_SCOPE(load_log);

	auto lock=data->readlock();

	std::optional<main_window_config::window_info_t> info;

	if (!lock->get_root())
		return info;

	auto xpath=lock->get_xpath(saved_element_to_xpath("window",
							  window_name));

	size_t n=xpath->count();

	if (n == 1)
	{
		xpath->to_node();

		try {
			info.emplace();

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
