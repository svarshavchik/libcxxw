/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include "pagefactory_impl.H"
#include "x/w/impl/container_element.H"
#include "x/w/impl/always_visible.H"
#include "x/w/impl/child_element.H"
#include "x/w/impl/singletonlayoutmanager.H"

LIBCXXW_NAMESPACE_START

pagefactoryObj::pagefactoryObj(const ref<implObj> &impl)
	: impl(impl)
{
}

pagefactoryObj::~pagefactoryObj()=default;

container_impl pagefactoryObj::get_container_impl()
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
	//
	// But override its initially_visible parameter to false, so that
	// the initial state is still disabled.
	implObj::info_t::lock lock{impl->info};

	container_impl factory_container_impl=
		ref<always_visibleObj<container_elementObj<child_elementObj>,
				      false>>
		::create(impl->lm->layoutmanagerObj::impl
			 ->layout_container_impl);

	lock->prev_container_impl=factory_container_impl;

	return factory_container_impl;
}

elementObj::implObj &pagefactoryObj::get_element_impl()
{
	return impl->lm->layoutmanagerObj::impl->get_element_impl();
}

container_impl pagefactoryObj::last_container_impl()
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

namespace {
#if 0
}
#endif

//! Override singletonlayoutmanager's recalculate()

//! This is the singleton layout manager for the container shim that's the
//! parent container for each page. The page layout manager uses this container
//! to effect showing and hiding the pages, by setting the container's size
//! to 0, and by managing the size and the position of each individual
//! page element.
//!
//! Override recalculate(), and forward it to the parent page layout manager.
//! The shim's recalculate() gets triggered when the page element's metrics
//! change, and the page layout manager will factor this in.

class dummy_singletonlayoutmanagerObj :
	public singletonlayoutmanagerObj::implObj {

	// Weakly capture the page layout manager, to avoid circular refs.

	const weakptr<ptr<pagelayoutmanagerObj::implObj>> lm_impl;

public:
	//! Constructor
	dummy_singletonlayoutmanagerObj(const container_impl &container_impl,
					const element &initial_element,
					const ref<pagelayoutmanagerObj::implObj>
					&lm_impl)
		: singletonlayoutmanagerObj::implObj{container_impl,
						     initial_element,
						     halign::fill,
						     valign::fill},
		  lm_impl{lm_impl}
	{
	}

private:
	void recalculate(ONLY IN_THREAD, const element_impl &) override
	{
		auto lm=lm_impl.getptr();
		if (lm)
			lm->recalculate(IN_THREAD);
	}
};

#if 0
{
#endif
}

void pagefactoryObj::created(const element &e)
{
	implObj::info_t::lock lock{impl->info};

	if (!lock->prev_container_impl)
		throw EXCEPTION("Internal error: unexpected call to "
				"pagefactoryObj::created()");

	// Make sure that any thrown exception, after construction of the
	// container object, destroys the implementation object too.

	container_impl container_impl=lock->prev_container_impl;
	lock->prev_container_impl=nullptr;

	// Finish the job started in get_container_impl(), above.

	auto lm_impl=ref<dummy_singletonlayoutmanagerObj>
		::create(container_impl, e, impl->lm->impl);
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
