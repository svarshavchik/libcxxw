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
#include <x/xml/escape.H>
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

const std::string_view libcxx_uri="https://www.libcxx.org/w";

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

				if (lock->name() == "windows" &&
				    lock->uri() == libcxx_uri &&
				    lock->get_attribute(
					    "version",
					    libcxx_uri
				    ) == version)
				{
					return d;
				}
			} catch (const exception &e)
			{
				throw EXCEPTION(filename << ": " << e);
			}
		} CATCH_EXCEPTIONS;
	}

	auto d=xml::doc::create();

	auto l=d->writelock();

	l->create_child()->element({"windows",
			"libcxx",
			libcxx_uri})->attribute({"version",
					libcxx_uri,
					version});

	return d;
}

screen_positionsObj::implObj::implObj(const std::string &filename,
				      const std::string &version)
	: appid{x::appid()}, filename{filename}, version{version},
	  data{load(filename, version)},
	  current_main_window_handlers{current_main_window_handlers_t::create()
	  },
	  widget_type_cache{widget_type_cache_t::create()}
{
}

screen_positions_handle screen_positionsObj::implObj::config_handle(
	const std::vector<std::string> &window_path,
	const std::string_view &ns,
	const std::string_view &type,
	const std::string_view &name
)
{
	auto lock=data->writelock();

	lock->get_root();

	std::string p;

	p.reserve(type.size()+ns.size()+name.size()+40);

	p = "ns:";
	p += type;
	p += "[libcxx:name=";
	p += xml::xpathescapestr(name);
	p += "]";

	std::string s;

	size_t n=p.size() + 10;

	for (const auto &p:window_path)
		n += sizeof("libcxx:window[libcxx:name=]/")+10
			+ p.size();
	s.reserve(n);

	std::string xpath_str;

	const char *sep="";

	for (const auto &p:window_path)
	{
		s += sep;
		s += "libcxx:window[";
		s += "libcxx:name=";
		s += xml::xpathescapestr(p);
		s += "]";
		sep="/";
	}

	if (s.size())
		lock->get_xpath(s, {
				{
					"libcxx", libcxx_uri
				}
			})->to_node();

	s += sep;
	s += p;

	auto xpath=lock->get_xpath(p, {
			{ "ns", ns },
			{ "libcxx", libcxx_uri }
		});

	n=xpath->count();

	if (n == 0)
	{
		if (ns == libcxx_uri)
			lock->create_child()->element({type, ns});
		else
			lock->create_child()->element({type, "ns", ns});

		lock->create_child()->element(
			{
				"name",
				libcxx_uri
			})->text(name)->parent()->parent();
	}

	std::string ns_s{ns};
	std::string type_s{type};

	return screen_positions_handle::create(
		ref{this}, s,
		widget_type_cache->find_or_create(
			{ns_s, type_s},
			[&]
			{
				return ref<widget_typeObj>::create(
					ns_s,
					type_s
				);
			}),
		name);
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

		std::string prev;
		bool first=true;

		// Sanity check:

		for (const auto &[key, mcguffin]
			     : *handler->unique_widget_labels)
		{
			auto p=mcguffin.getptr();

			if (!p)
				continue;

			if (!first && prev == key)
				throw EXCEPTION("Duplicate widget label: \""
						<< prev
						<< "\"");
			prev=key;
			first=false;
		}

		// Save any save-able widgets in the window, first.

		handler->save(IN_THREAD);

		// Now save the current window position.

		std::vector<std::string> window_path;

		handler->window_id_hierarchy(window_path);

		std::string window_id=window_path.back();

		window_path.pop_back();

		// Not newconfig, we don't want to remove the whole kit and
		// kaboodle.
		auto lock=handler->config_handle->newconfig(false);

		// Remove any previous <position>
		{
			auto xpath=lock->get_xpath("position");

			if (xpath->count())
			{
				xpath->to_node();
				lock->remove();
			}
		}
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

std::optional<window_position_t> find_window_position(
	const screen_positions_handle &config_handle
)
{
	LOG_FUNC_SCOPE(load_log);

	std::optional<window_position_t> info;

	auto lock=config_handle->config();

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

screen_positions_handle generic_windowObj::handlerObj::widget_config_handle(
	const std::string_view &ns,
	const std::string_view &type,
	const std::string_view &name,
	const ref<obj> &mcguffin
) const
{
	std::vector<std::string> window_path;

	window_id_hierarchy(window_path);

	std::string s;

	s.reserve(type.size() + name.size() + 1);

	s=std::string{type};
	s += ":";
	s += name;

	unique_widget_labels->insert(s, mcguffin);

	return positions->impl->config_handle(
		window_path,
		ns,
		type,
		name);
}

void preserve_screen_number(bool flag)
{
	property::load_property(LIBCXX_NAMESPACE_STR
				"::w::preserve_screen_number",
				flag ? "true":"false", true, true);
}

LIBCXXW_NAMESPACE_END
