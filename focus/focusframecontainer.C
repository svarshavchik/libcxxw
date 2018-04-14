/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/impl/focus/focusable.H"
#include "focus/focusframecontainer.H"
#include "focus/focusframecontainer_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/container.H"
#include "gridlayoutmanager.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

focusframecontainer
create_focusframecontainer(const ref<focusframecontainerObj::implObj> &impl,
			   const element &e,
			   const focusable_impl &element_focusable_impl)
{
	auto ffc=focusframecontainer::create(impl, element_focusable_impl);

	install_focusframe_element(ffc, e);

	return ffc;
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


// Temporary container for the new focus frame's layout manager.

struct LIBCXX_HIDDEN focusframecontainerObj::new_focusframelayoutmanager {

	ref<focusframelayoutimplObj> new_layoutmanager;
};

// Main constructor: create a focusframelayoutimplObj instance, then invoke
// the delegated constructor

focusframecontainerObj::focusframecontainerObj(const ref<implObj> &impl,
					       const focusable_impl
					       &focusable_impl)
	: focusframecontainerObj(impl,
				 ref(&impl->get_container_impl()),
				 focusable_impl,
				 new_focusframelayoutmanager{
					 ref<focusframelayoutimplObj>::create
						 (impl)})
{
}

// The delegated constructor constructs the container superclass, using
// the new focusframelayoutimplObj layout manager, and finishes constructing
// the object. It can now construct the implementation object, and save
// a ref to the grid layout manager subclass in it.

focusframecontainerObj::focusframecontainerObj(const ref<implObj> &impl,
					       const container_impl
					       &container_impl,
					       const focusable_impl
					       &focusable_impl,
					       const new_focusframelayoutmanager
					       &factory)

	: containerObj(container_impl,
		       factory.new_layoutmanager),
	  focusableObj::ownerObj(focusable_impl),
	  impl(impl)
{
}

focusframecontainerObj::~focusframecontainerObj()=default;

LIBCXXW_NAMESPACE_END
