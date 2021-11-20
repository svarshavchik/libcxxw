/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/focusable_container.H"
#include "x/w/new_focusable_layoutmanagerfwd.H"
#include "x/w/factory.H"
#include "x/w/impl/container.H"
#include "x/w/input_field.H"
#include "x/w/validated_input_field.H"
#include "messages.H"
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

new_focusable_layoutmanager::new_focusable_layoutmanager()=default;

new_focusable_layoutmanager::~new_focusable_layoutmanager()=default;

std::tuple<focusable_container, input_field>
new_focusable_layoutmanager::create(
	const container_impl &parent,
	const function<void (const focusable_container &)> &creator,
	const text_param &initial_contents,
	const input_field_validation_callback &callback,
	bool validated
) const
{
	throw EXCEPTION(_("Wrong layout manager specified for "
			  "create_focusable_container()"));
}

focusable_container factoryObj::do_create_focusable_container(
	const function<void (const focusable_container &)> &creator,
	const new_focusable_layoutmanager &layout_manager)
{
	auto c=layout_manager.create(get_container_impl(), creator);
	created(c);
	return c;
}

std::tuple<focusable_container, input_field>
factoryObj::do_create_focusable_container(
	const function<void (const focusable_container &)> &creator,
	const new_focusable_layoutmanager &layout_manager,
	const text_param &t,
	const input_field_validation_callback &callback,
	bool validated
)
{
	auto ret=layout_manager.create(get_container_impl(), creator,
				       t, callback, validated);
	created(std::get<0>(ret));
	return ret;
}

LIBCXXW_NAMESPACE_END
