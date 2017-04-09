/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "focus/focusable.H"
#include "focus/focusframecontainer.H"
#include "focus/focusframecontainer_impl.H"
#include "focus/focusframelayoutimpl.H"
#include "new_layoutmanager.H"
#include "x/w/gridfactory.H"
#include "container.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

// Temporary container for the new focus frame's layout manager.

struct LIBCXX_HIDDEN focusframecontainerObj::new_focusframelayoutmanager {

	ref<focusframelayoutimplObj> new_layoutmanager;
};

// Main constructor: create a focusframelayoutimplObj instance, then invoke
// the delegated constructor

focusframecontainerObj::focusframecontainerObj(const ref<implObj> &impl,
					       const ref<focusableImplObj>
					       &focusable_impl)
	: focusframecontainerObj(impl, focusable_impl,
				 new_focusframelayoutmanager{
					 ref<focusframelayoutimplObj>
						 ::create(impl)})
{
}

// The delegated constructor constructs the container superclass, using
// the new focusframelayoutimplObj layout manager, and finishes constructing
// the object. It can now construct the implementation object, and save
// a ref to the grid layout manager subclass in it.

focusframecontainerObj::focusframecontainerObj(const ref<implObj> &impl,
					       const ref<focusableImplObj>
					       &focusable_impl,
					       const new_focusframelayoutmanager
					       &factory)

	: containerObj(impl, factory.new_layoutmanager),
	  focusableObj::ownerObj(focusable_impl),
	  impl(impl)
{
}

focusframecontainerObj::~focusframecontainerObj()=default;

///////////////////////////////////////////////////////////////////////////
//
// set_focusable() returns a private factory object, whose created() installs
// the new display element into the real, underlying grid factory.

class LIBCXX_HIDDEN focusframelayoutmanagerObj : public factoryObj {

 public:

	const focusframecontainer ffc;

	focusframelayoutmanagerObj(const focusframecontainer &ffc)
		: factoryObj(ffc->impl),
		ffc(ffc)
		{
		}

	void created(const element &e) override
	{
		gridlayoutmanager glm=ffc->get_layoutmanager();

		glm->erase();

		glm->append_row()->padding(0).created_internally(e);
	}
};

factory focusframecontainerObj::set_focusable()
{
	return ref<focusframelayoutmanagerObj>::create
		(ref<focusframecontainerObj>(this));
}

LIBCXXW_NAMESPACE_END
