/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "container.H"
#include "x/w/container.H"
#include "x/w/factory.H"
#include "new_layoutmanager.H"
#include "layoutmanager.H"
#include "container_element.H"
#include "child_element.H"

LIBCXXW_NAMESPACE_START

containerObj::containerObj(const ref<implObj> &impl,
			   const new_layoutmanager &layout_factory)
	: containerObj(impl,
		       layout_factory.create({impl}).layout_manager_impl)
{
}

containerObj::containerObj(const ref<implObj> &impl,
			   const ref<layoutmanagerObj::implObj> layout_impl)
	: elementObj(impl),
	  impl(impl),
	  layout_impl(layout_impl)
{
	impl->install_layoutmanager(layout_impl);
}

containerObj::~containerObj()
{
	impl->uninstall_layoutmanager();
}

layoutmanager containerObj::get_layoutmanager()
{
	return layout_impl->create_public_object();
}

const_layoutmanager containerObj::get_layoutmanager() const
{
	return layout_impl->create_public_object();
}

container factoryObj
::do_create_container(const function<void (const container &)> &creator,
		      const new_layoutmanager &layout_manager)
{
	auto c=container::create(ref<container_elementObj<child_elementObj>>
				 ::create(container_impl,
					  metrics::horizvert_axi(),
					  "background@libcxx"),
				 layout_manager);

	creator(c);
	created(c);
	return c;
}

LIBCXXW_NAMESPACE_END
