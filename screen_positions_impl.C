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
	const std::string_view &ns,
	const std::string_view &type,
	const std::string_view &name_s
)
{
	auto lock=data->writelock();
	lock->get_root();

	auto my_ns=lock->prefix();

	for (const auto &p:window_path)
	{
		std::string s;

		s.reserve(my_ns.size()*2 + p.size() + 100);

		s=my_ns;
		s += ":window[";
		s += my_ns;
		s += ":name=";
		s += xml::quote_string_literal(p);
		s += "]";

		auto xpath=lock->get_xpath(s);

		if (xpath->count())
			xpath->to_node();
		else
		{
			lock->create_child()->element({"window",
					libcxx_uri})
				->element({"name", libcxx_uri})->text(p)
				->parent()->parent();
		}
	}

	std::string s;

	s.reserve(type.size()+ns.size()+name_s.size()+100);

	s = "ns:";
	s += type;
	s += "[libcxx:name=";
	s += xml::quote_string_literal(name_s);
	s += "]";

	auto xpath=lock->get_xpath(s, {
			{ "ns", ns },
			{ "libcxx", libcxx_uri }
		});

	size_t n=xpath->count();

	if (type == "window" && ns == libcxx_uri && n)
	{
		// Do not remove the existing <window> node, since it may
		// have inferiors that we want to keep.

		xpath->to_node();
		return lock;
	}

	if (n)
	{
		xpath->to_node();
		lock->remove();
	}

	if (ns == libcxx_uri)
		lock->create_child()->element({type, ns});
	else
		lock->create_child()->element({type, "ns", ns});

	lock->create_child()->element(
		{
			"name",
			libcxx_uri
		})->text(name_s)->parent()->parent();

	return lock;
}

xml::readlockptr screen_positionsObj::implObj::create_readlock_for_loading(
	const std::vector<std::string> &window_path,
	const std::string_view &ns,
	const std::string_view &type,
	const std::string_view &name
) const
{
	size_t l=type.size()+name.size()+50;

	for (const auto &p:window_path)
		l += p.size()+20;

	std::string s;

	s.reserve(l);
	for (const auto &p:window_path)
	{
		s += "libcxx:window[libcxx:name=";
		s += xml::quote_string_literal(p);
		s += "]/";
	}

	s += "ns:";
	s += type;
	s += "[libcxx:name=";
	s += xml::quote_string_literal(name) + "]";

	auto lock=data->readlock();

	lock->get_root();

	auto xpath=lock->get_xpath(s, {
			{ "libcxx", libcxx_uri },
			{ "ns", ns },
		});

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

		handler->save(IN_THREAD, handler->positions);

		// Now save the current window position.

		std::vector<std::string> window_path;

		handler->window_id_hierarchy(window_path);

		std::string window_id=window_path.back();

		window_path.pop_back();

		auto lock=handler->positions->impl
			->create_writelock_for_saving(window_path,
						      libcxx_uri,
						      "window",
						      window_id);

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

std::optional<window_position_t>
screen_positionsObj::implObj::find_window_position(
	const std::vector<std::string> &parent_windows,
	const std::string_view &window_name
) const
{
	LOG_FUNC_SCOPE(load_log);

	std::optional<window_position_t> info;

	auto lockptr=create_readlock_for_loading(parent_windows,
						 libcxx_uri,
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

void generic_windowObj::handlerObj::register_unique_widget_label(
	const std::string &label,
	const ref<obj> &mcguffin)
{
	unique_widget_labels->insert(label, mcguffin);
}

void preserve_screen_number(bool flag)
{
	property::load_property(LIBCXX_NAMESPACE_STR
				"::w::preserve_screen_number",
				flag ? "true":"false", true, true);
}

LIBCXXW_NAMESPACE_END
