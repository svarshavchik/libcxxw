/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/new_layoutmanager.H"
#include "container.H"
#include "layoutmanager.H"

LIBCXXW_NAMESPACE_START

new_layoutmanagerObj::new_layoutmanagerObj()=default;

new_layoutmanagerObj::~new_layoutmanagerObj()=default;

// TODO

class LIBCXX_HIDDEN dummy_layoutmanager_factoryObj : public new_layoutmanagerObj
{
 public:

	dummy_layoutmanager_factoryObj()=default;

	~dummy_layoutmanager_factoryObj()=default;

	ref<layoutmanagerObj::implObj>
		create(const ref<containerObj::implObj> &container_impl)
		override
	{
		return ref<layoutmanagerObj::implObj>::create
			(container_impl);
	}
};

new_layoutmanager new_layoutmanagerBase::create_grid()
{
	return ref<dummy_layoutmanager_factoryObj>::create();
}

LIBCXXW_NAMESPACE_END
