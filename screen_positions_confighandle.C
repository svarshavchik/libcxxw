/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_positions_impl.H"
#include "x/w/impl/screen_positions_confighandle.H"
#include <x/xml/xpath.H>

LIBCXXW_NAMESPACE_START

screen_positionsObj::config_handleObj::config_handleObj(
	const ref<implObj> &impl,
	const std::string &xpath,
	const ref<widget_typeObj> &widget_type,
	const std::string_view &name)
	: impl{impl},
	  xpath{xpath},
	  widget_type{widget_type}, name{name}
{
}

screen_positionsObj::config_handleObj::~config_handleObj()=default;

void screen_positionsObj::config_handleObj::position(const xml::readlock &lock)
	const
{
	lock->get_root();

	lock->get_xpath(xpath, {
			{ "libcxx", libcxx_uri},
			{ "ns", widget_type->ns}
		})->to_node();
}

xml::readlock screen_positionsObj::config_handleObj::config() const
{
	// main_windows use config(), but they want a writelock.
	auto lock=impl->data->readlock();

	position(lock);

	return lock;
}

xml::writelock screen_positionsObj::config_handleObj::newconfig(
	bool remove_existing
)
{
	auto lock=impl->data->writelock();

	position(lock);

	if (!remove_existing)
		return lock;

	lock->remove();

	if (widget_type->ns == libcxx_uri)
		lock->create_child()->element({widget_type->type,
				widget_type->ns});
	else
		lock->create_child()->element({widget_type->type,
				"ns", widget_type->ns});

	lock->create_child()->element(
		{
			"name",
			libcxx_uri
		})->text(name)->parent()->parent();

	return lock;
}

LIBCXXW_NAMESPACE_END
