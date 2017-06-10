/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "listlayoutmanager/listlayoutmanager.H"
#include "listlayoutmanager/listitemfactoryobj.H"
#include "listlayoutmanager/listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

//! Default list padding

list_padding default_list_padding()
{
	return {"list_v_padding",
			"list_left_padding",
			"list_inner_padding",
			"list_right_padding"
			};
}

container create_listlayoutmanager_impl
(const factory &f,
 size_t columns,
 const listlayoutstyle &style,
 const list_padding &padding,
 const function<void (const container &)> &builder)
{
	auto impl=ref<listcontainerObj::implObj>
		::create(f->container_impl, padding);

	auto c=listcontainer::create
		(impl,
		 ref<listlayoutmanagerObj::implObj>::create
		 (impl, style, columns));

	builder(c);
	f->created_internally(c);

	return c;
}

listlayoutmanagerObj::listlayoutmanagerObj(const ref<implObj> &impl)
	: gridlayoutmanagerObj(impl),
	  impl(impl)
{
}

listlayoutmanagerObj::~listlayoutmanagerObj()=default;

class LIBCXX_HIDDEN list_append_item_factoryObj : public listitemfactoryObj {
 public:

	using listitemfactoryObj::listitemfactoryObj;

	gridfactory create_factory() const override
	{
		return me->append_row();
	}
};

factory listlayoutmanagerObj::append_item()
{
	return ref<list_append_item_factoryObj>
		::create(listlayoutmanager(this));
}

LIBCXXW_NAMESPACE_END
