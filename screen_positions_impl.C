/*
** Copyright 2018-2021 Double Precision, Inc.
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
#include <x/appid.H>

#include <unistd.h>
#include <sstream>

LIBCXXW_NAMESPACE_START

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE::w::load_screen_positions, load_log);

static property::value<bool>
preserve_screen_number_prop(LIBCXX_NAMESPACE_STR "::w::preserve_screen_number",
			    true);

screen_positionsObj::implObj::~implObj()=default;


void screen_positionsObj::implObj::save()
{
	// Make sure to wait for the connection thread to finish saving
	// individual window data.

	auto lock=create_unique();

	data->readlock()->save_file(filename);
}

static auto load(const std::string &filename,
		 const std::string &version)
{
	LOG_FUNC_SCOPE(load_log);

	if (access(filename.c_str(), R_OK) == 0)
	{
		try
		{
			try {
				auto d=xml::doc::create(filename);

				auto lock=d->readlock();

				lock->get_root();

				lock->get_xpath("/windows")->to_node();

				auto s=lock->get_any_attribute("version");

				if (s == version)
					return d;

			} catch (const exception &e)
			{
				throw EXCEPTION(filename << ": " << e);
			}
		} CATCH_EXCEPTIONS;
	}

	auto d=xml::doc::create();

	auto l=d->writelock();

	l->create_child()->element({"windows"})->attribute({"version",
			version});

	return d;
}

screen_positionsObj::implObj::implObj(const std::string &filename,
				      const std::string &version)
	: appid{x::appid()}, filename{filename}, version{version},
	  data{load(filename, version)},
	  current_main_window_handlers{current_main_window_handlers_t::create()}
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
	s += xml::quote_string_literal(name);
	s += "]";

	return s;
}

xml::writelock screen_positionsObj::implObj::create_writelock_for_saving(
	const std::vector<std::string> &window_path,
	const std::string_view &type,
	const std::string_view &name_s
)
{
	auto lock=data->writelock();
	lock->get_root();

	lock->get_xpath("/windows")->to_node();

	for (const auto &p:window_path)
	{
		auto xpath=lock->get_xpath("window[name="
					   + xml::quote_string_literal(p)
					   + "]");

		if (xpath->count())
			xpath->to_node();
		else
		{
			lock->create_child()->element({"window"})
				->element({"name"})->text(p)
				->parent()->parent();
		}
	}

	std::string s;

	s.reserve(type.size()+name_s.size()+20);

	s = type;
	s += "[name=";
	s += xml::quote_string_literal(name_s);
	s += "]";

	auto xpath=lock->get_xpath(s);

	size_t n=xpath->count();

	for (size_t i=1; i <= n; ++i)
	{
		xpath->to_node(i);
		lock->remove();
	}

	lock->create_child()->element({std::string{type}})
		->element({"name"})->text(name_s)->parent()->parent();

	return lock;
}

xml::readlockptr screen_positionsObj::implObj::create_readlock_for_loading(
	const std::vector<std::string> &window_path,
	const std::string_view &type,
	const std::string_view &name
) const
{
	size_t l=type.size()+name.size()+40;

	for (const auto &p:window_path)
		l += p.size()+20;

	std::string s{"/windows"};

	for (const auto &p:window_path)
		s += "/window[name=" + xml::quote_string_literal(p) + "]";

	s += "/";
	s += type;
	s += "[name=" + xml::quote_string_literal(name) + "]";

	auto lock=data->readlock();

	lock->get_root();

	auto xpath=lock->get_xpath(s);

	if (xpath->count())
	{
		xpath->to_node();
		return lock;
	}
	return {};
}

main_windowObj::~main_windowObj()
{
	if (impl->handler->window_id.empty())
		return;

	in_thread([impl=this->impl,
		   lock=impl->handler->positions->impl->create_shared()]
		  (ONLY IN_THREAD)
	{
		auto handler=impl->handler;

		auto [wx, wy] = handler->root_xy.get();

		auto r=handler->current_position.get();

		// A window or a dialog can be created but never shown, so its
		// screen position will not get initialized. Avoid saving this
		// window's coordinates, in this case.

		if (!handler->has_exposed(IN_THREAD))
			return;

		// Save any save-able widgets in the window, first.

		handler->save(IN_THREAD, handler->positions);

		// Now save the current window position.

		std::vector<std::string> window_path;

		handler->window_id_hierarchy(window_path);

		std::string window_id=window_path.back();

		window_path.pop_back();

		auto lock=handler->positions->impl
			->create_writelock_for_saving(window_path,
						      "window",
						      window_id);

		auto window=lock->create_child()->element({"position"});

		window=window->create_child()->element({"x"})->text(wx);
		window=window->parent()->create_next_sibling()->element({"y"})
			->create_child()->text(wy);
		window=window->parent()->create_next_sibling()
			->element({"width"})->create_child()->text(r.width);
		window->parent()->create_next_sibling()->element({"height"})
			->create_child()->text(r.height);

		if (preserve_screen_number_prop.get())
		{
			window=window->parent()->create_next_sibling()
				->element({"screen"})
				->create_child()->text(
					handler->get_screen()->impl
					->screen_number
				);
		}
	});
}

std::optional<window_position_t>
screen_positionsObj::implObj::find_window_position(
	const std::vector<std::string> &parent_windows,
	const std::string_view &window_name
) const
{
	LOG_FUNC_SCOPE(load_log);

	std::optional<window_position_t> info;

	auto lockptr=create_readlock_for_loading(parent_windows,
						 "window",
						 window_name);

	if (!lockptr)
		return info;

	xml::readlock lock{lockptr};

	auto xpath=lock->get_xpath("position");

	if (xpath->count())
	{
		xpath->to_node();

		try {
			info.emplace();

			auto &r=info->coordinates;

			auto value=lock->clone();

			value->get_xpath("x")->to_node();

			r.x=value->get_text<coord_t>();

			value=lock->clone();

			value->get_xpath("y")->to_node();

			r.y=value->get_text<coord_t>();

			value=lock->clone();

			value->get_xpath("width")->to_node();

			r.width=value->get_text<dim_t>();

			value=lock->clone();

			value->get_xpath("height")->to_node();

			r.height=value->get_text<dim_t>();

			value=lock->clone();

			auto xpath=value->get_xpath("screen");

			if (xpath->count() == 1 &&
			    preserve_screen_number_prop.get())
			{
				xpath->to_node();

				info->screen_number=value->get_text<size_t>();
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
