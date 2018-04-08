/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusable.H"
#include "focus/focusframefactory.H"
#include "focus/focusframecontainer.H"
#include "focus/focusframecontainer_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "x/w/impl/container.H"
#include "gridlayoutmanager.H"
#include "current_border_impl.H"

LIBCXXW_NAMESPACE_START

// Temporary container for the new focus frame's layout manager.

struct LIBCXX_HIDDEN focusframecontainerObj::new_focusframelayoutmanager {

	ref<focusframelayoutimplObj> new_layoutmanager;
};

// Main constructor: create a focusframelayoutimplObj instance, then invoke
// the delegated constructor

focusframecontainerObj::focusframecontainerObj(const ref<implObj> &impl,
					       const ref<focusableObj::implObj>
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
					       const ref<containerObj::implObj>
					       &container_impl,
					       const ref<focusableObj::implObj>
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

///////////////////////////////////////////////////////////////////////////
//
// set_focusable() returns a private factory object, whose created() installs
// the new display element into the real, underlying grid factory.

factory focusframecontainerObj::set_focusable()
{
	return focusframefactory::create(container(this));
}

element focusframecontainerObj::get_focusable() const
{
	const_gridlayoutmanager glm=get_layoutmanager();

	return glm->get(0, 0);
}

LIBCXXW_NAMESPACE_END
