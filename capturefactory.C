/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "capturefactory.H"
#include "x/w/element.H"
#include "messages.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

capturefactoryObj::capturefactoryObj(const ref<containerObj::implObj>
				     &container_impl)
	: container_impl(container_impl)
{
}

capturefactoryObj::~capturefactoryObj()=default;

ref<containerObj::implObj> capturefactoryObj::get_container_impl()
{
	return container_impl;
}

elementObj::implObj &capturefactoryObj::get_element_impl()
{
	return container_impl->get_element_impl();
}

void capturefactoryObj::created(const element &e)
{
	capture_element_t::lock lock{capture_element};

	if (*lock)
		throw EXCEPTION(_("Only one display element can be created"));

	*lock=e;
}

element capturefactoryObj::get()
{
	capture_element_t::lock lock{capture_element};

	if (!*lock)
		throw EXCEPTION(_("No display element was created by the factory"));

	return *lock;
}

LIBCXXW_NAMESPACE_END
