/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/borderlayoutmanager.H"
#include "x/w/impl/bordercontainer_element.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/child_element.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/container.H"
#include "x/w/impl/container.H"
#include "capturefactory.H"

LIBCXXW_NAMESPACE_START

borderlayoutmanagerObj::borderlayoutmanagerObj(const ref<implObj> &impl)
	: singletonlayoutmanagerObj{impl}, impl{impl}
{
}

borderlayoutmanagerObj::~borderlayoutmanagerObj()=default;

container factoryObj
::do_create_bordered_element(const function<void (const factory &)>
			     &creator,
			     const border_arg &border,
			     const dim_arg &hpad,
			     const dim_arg &vpad)
{
	// Retrieve my parent, and create the container for the
	// border layout manager.
	auto parent=get_container_impl();

	auto c_impl=ref<bordercontainer_elementObj<container_elementObj
						   <child_elementObj>>>
		::create(border, border, border, border,
			 hpad, vpad,
			 parent);

	// Invoke the creator to set the element for which we're providing
	// the border.
	auto f=capturefactory::create(c_impl);

	creator(f);

	auto e=f->get();

	// Finish everything up.
	auto lm_impl=ref<borderlayoutmanagerObj::implObj>::create(c_impl,
								  c_impl,
								  e);
	auto new_container=container::create(c_impl, lm_impl);
	created(new_container);
	return new_container;
}

LIBCXXW_NAMESPACE_END
