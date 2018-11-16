/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/standard_focusframecontainer_element.H"
#include "x/w/impl/focus/focusframecontainer_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/container.H"
#include "gridlayoutmanager.H"
#include "x/w/impl/current_border_impl.H"

LIBCXXW_NAMESPACE_START

focusable_container_owner
create_focusframe_container_owner(const container_impl &parent_container,
				  const ref<focusframecontainer_implObj> &impl,
				  const element &e,
				  const focusable_impl &element_focusable_impl)
{
	auto lm=ref<focusframelayoutimplObj>::create(parent_container,
						     impl, e);

	auto c=focusable_container_owner::create(impl, lm,
						 element_focusable_impl);

	lm->needs_recalculation();
	return c;
}

container create_focusframe_container(const
				      ref<focusframecontainer_implObj> &impl,
				      const element &e)
{
	auto lm=ref<focusframelayoutimplObj>::create(impl, impl, e);

	auto c=container::create(lm->layout_container_impl, lm);

	return c;
}

LIBCXXW_NAMESPACE_END
