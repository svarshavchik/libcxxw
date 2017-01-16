/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/new_layoutmanager.H"
#include "container.H"
#include "layoutmanager.H"
#include "gridlayoutmanager.H"

LIBCXXW_NAMESPACE_START

new_layoutmanagerObj::new_layoutmanagerObj()=default;

new_layoutmanagerObj::~new_layoutmanagerObj()=default;

class LIBCXX_HIDDEN grid_factoryObj : public new_layoutmanagerObj
{
 public:

	grid_factoryObj()=default;

	~grid_factoryObj()=default;

	ref<layoutmanagerObj::implObj>
		create(const ref<containerObj::implObj> &container_impl)
		override
	{
		return ref<gridlayoutmanagerObj::implObj>::create(container_impl);
	}
};

new_layoutmanager new_layoutmanagerBase::create_grid()
{
	return ref<grid_factoryObj>::create();
}

LIBCXXW_NAMESPACE_END
