/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "pagefactory_impl.H"
#include "container_element.H"
#include "always_visible.H"
#include "child_element.H"
#include "singletonlayoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

pagefactoryObj::pagefactoryObj(const ref<implObj> &impl)
	: impl(impl)
{
}

pagefactoryObj::~pagefactoryObj()=default;

ref<containerObj::implObj> pagefactoryObj::get_container_impl()
{
	// Pull a switcheroo that creates a new container that uses the
	// singleton layout manager. This is what gets added to the
	// switched container, and the pagelayoutmanager does its thing
	// by controlling this new container's visibility.
	//
	// The actual element being created goes into this new container,
	// and the container gets added to the switched container. In this
	// manner, the page layout manager does its job without affecting
	// the explicit visibilit of the created display element.
	//
	// Use the always_visible mixin, so that recursive show/hide_all()s
	// propagate through the intermediate container, having no effect.
	implObj::info_t::lock lock{impl->info};

	ref<containerObj::implObj> container_impl=
		ref<always_visibleObj<container_elementObj<child_elementObj>>>
		::create(impl->lm->layoutmanagerObj::impl->container_impl);

	lock->prev_container_impl=container_impl;

	return container_impl;
}

elementObj::implObj &pagefactoryObj::get_element_impl()
{
	return impl->lm->layoutmanagerObj::impl->get_element_impl();
}

ref<containerObj::implObj> pagefactoryObj::last_container_impl()
{
	implObj::info_t::lock lock{impl->info};

	if (!lock->prev_container_impl)
		throw EXCEPTION("Internal error: unexpected call to "
				"pagefactoryObj::last_container_impl()");

	return lock->prev_container_impl;
}

pagefactoryObj &pagefactoryObj::halign(LIBCXXW_NAMESPACE::halign a)
{
	implObj::info_t::lock lock{impl->info};

	lock->horizontal_alignment=a;

	return *this;
}

pagefactoryObj &pagefactoryObj::valign(LIBCXXW_NAMESPACE::valign a)
{
	implObj::info_t::lock lock{impl->info};

	lock->vertical_alignment=a;

	return *this;
}

void pagefactoryObj::created(const element &e)
{
	implObj::info_t::lock lock{impl->info};

	if (!lock->prev_container_impl)
		throw EXCEPTION("Internal error: unexpected call to "
				"pagefactoryObj::created()");

	// Make sure that any thrown exception, after construction of the
	// container object, destroys the implementation object too.

	ref<containerObj::implObj> container_impl=lock->prev_container_impl;
	lock->prev_container_impl=nullptr;

	// Finish the job started in get_container_impl(), above.

	auto lm_impl=ref<singletonlayoutmanagerObj::implObj>
		::create(container_impl, e);
	auto c=container::create(container_impl, lm_impl);

	// Reset the factory's locked element properties after constructing
	// the switch_element_info, so that if created_under_lock() blows up,
	// the factory is in the reset condition.

	switch_element_info info{
		c, e, lock->horizontal_alignment, lock->vertical_alignment};

	*lock={};

	impl->created_under_lock(info);
}

LIBCXXW_NAMESPACE_END
