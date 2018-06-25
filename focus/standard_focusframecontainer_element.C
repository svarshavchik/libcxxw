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
create_focusframe_container_owner(const ref<focusframecontainer_implObj> &impl,
				  const element &e,
				  const focusable_impl &element_focusable_impl)
{
	auto lm=ref<focusframelayoutimplObj>::create(impl, e);

	auto c=focusable_container_owner::create(impl, lm,
						 element_focusable_impl);

	install_focusframe_element(c, e);

	return c;
}

container create_focusframe_container(const
				      ref<focusframecontainer_implObj> &impl,
				      const element &e)
{
	auto lm=ref<focusframelayoutimplObj>::create(impl, e);

	auto c=container::create(lm->layout_container_impl, lm);

	install_focusframe_element(c, e);

	return c;
}

void install_focusframe_element(const container &c,
				const element &e)
{
	gridlayoutmanager glm=c->get_layoutmanager();

	// If the focusframe is inside a grid layout and the cell is filled
	// with the focus frame, return the courtesy by centering the contents
	// of the focus frame.
	glm->append_row()->padding(0)
		.halign(halign::center)
		.valign(valign::middle)
		.created_internally(e);
}

LIBCXXW_NAMESPACE_END
