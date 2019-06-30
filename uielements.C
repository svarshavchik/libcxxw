/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
#include "x/w/listitemhandle.H"
#include "messages.H"
#include "gridlayoutmanager.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/panelayoutmanager.H"
#include "x/w/impl/container.H"
#include "x/w/radio_group.H"
#include "x/w/synchronized_axis.H"
#include "x/w/copy_cut_paste_menu_items.H"
#include "screen.H"
#include "defaulttheme.H"

LIBCXXW_NAMESPACE_START

typedef uielements::new_radio_groups_t new_radio_groups_t;

new_radio_groups_t::new_radio_groups_t()=default;

new_radio_groups_t::~new_radio_groups_t()=default;

new_radio_groups_t::new_radio_groups_t(const new_radio_groups_t &)=default;

new_radio_groups_t::new_radio_groups_t(new_radio_groups_t &&)=default;

new_radio_groups_t &new_radio_groups_t::operator=(const new_radio_groups_t &)
=default;

new_radio_groups_t &new_radio_groups_t::operator=(new_radio_groups_t &&)
=default;


typedef uielements::new_synchronized_axis_t new_synchronized_axis_t;

new_synchronized_axis_t::new_synchronized_axis_t()=default;

new_synchronized_axis_t::~new_synchronized_axis_t()=default;

new_synchronized_axis_t
::new_synchronized_axis_t(const new_synchronized_axis_t &)=default;

new_synchronized_axis_t
::new_synchronized_axis_t(new_synchronized_axis_t &&)=default;

new_synchronized_axis_t &
new_synchronized_axis_t::operator=(const new_synchronized_axis_t &)
=default;

new_synchronized_axis_t &
new_synchronized_axis_t::operator=(new_synchronized_axis_t &&)
=default;


uielements::~uielements()=default;

element uielements::get_element(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_elements.find(std::string{name.begin(), name.end()});

	if (iter == new_elements.end())
		throw EXCEPTION(gettextmsg(_("Element %1% was not found"),
					   name));

	return iter->second;
}


layoutmanager uielements::get_layoutmanager(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_layoutmanagers.find(std::string{name.begin(),
							      name.end()});

	if (iter == new_layoutmanagers.end())
		throw EXCEPTION(gettextmsg(_("Layout manager"
					     " %1% was not found"),
					   name));

	return iter->second;
}

radio_group uielements::get_radio_group(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_radio_groups.find(std::string{name.begin(), name.end()});

	if (iter == new_radio_groups.end())
		throw EXCEPTION(gettextmsg
				(_("Radio button group %1% was not found"),
				 name));

	return iter->second;
}

synchronized_axis uielements
::get_synchronized_axis(const std::string_view &name) const
{
	// TODO: C++20;

	auto iter=new_synchronized_axis.find(std::string{name.begin(),
								  name.end()});

	if (iter == new_synchronized_axis.end())
		throw EXCEPTION(gettextmsg
				(_("Radio button group %1% was not found"),
				 name));

	return iter->second;
}

/////////////////////////////////////////////////////////////////////////////

void gridlayoutmanagerObj::generate(const std::string_view &name,
				    const const_uigenerators &generators,
				    uielements &elements)
{
	// TODO: C++20
	auto iter=generators->gridlayoutmanager_generators.find({name.begin(),
								 name.end()});

	if (iter == generators->gridlayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

void booklayoutmanagerObj::generate(const std::string_view &name,
				    const const_uigenerators &generators,
				    uielements &elements)
{
	// TODO: C++20
	auto iter=generators->booklayoutmanager_generators.find({name.begin(),
								 name.end()});

	if (iter == generators->booklayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

void listlayoutmanagerObj::generate(const std::string_view &name,
				    const const_uigenerators &generators,
				    uielements &elements)
{
	// TODO: C++20
	auto iter=generators->listlayoutmanager_generators.find({name.begin(),
								 name.end()});

	if (iter == generators->listlayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

void panelayoutmanagerObj::generate(const std::string_view &name,
				    const const_uigenerators &generators,
				    uielements &elements)
{
	// TODO: C++20
	auto iter=generators->panelayoutmanager_generators.find({name.begin(),
								 name.end()});

	if (iter == generators->panelayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}


void standard_comboboxlayoutmanagerObj
::generate(const std::string_view &name,
	   const const_uigenerators &generators,
	   uielements &elements)
{
	// TODO: C++20
	auto iter=generators->standard_comboboxlayoutmanager_generators
		.find({name.begin(),
		       name.end()});

	if (iter == generators->standard_comboboxlayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

void editable_comboboxlayoutmanagerObj
::generate(const std::string_view &name,
	   const const_uigenerators &generators,
	   uielements &elements)
{
	// TODO: C++20
	auto iter=generators->editable_comboboxlayoutmanager_generators
		.find({name.begin(),
		       name.end()});

	if (iter == generators->editable_comboboxlayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

void menubarlayoutmanagerObj::generate(const std::string_view &name,
				       const const_uigenerators &generators,
				       uielements &elements)
{
	// TODO: C++20
	auto iter=generators->menubarlayoutmanager_generators
		.find({name.begin(), name.end()});

	if (iter == generators->menubarlayoutmanager_generators.end())
	{
		throw EXCEPTION(gettextmsg(_("Layout %1% not defined."),
					   name));
	}

	auto me=ref{this};

	for (const auto &g:*iter->second)
		g(me, elements);
}

LIBCXXW_NAMESPACE_END
