/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "booklayoutmanager/pagetabgridlayoutmanager_impl.H"
#include "booklayoutmanager/pagetabgridcontainer_impl.H"
#include "gridlayoutmanager_impl_elements.H"
#include "container.H"

LIBCXXW_NAMESPACE_START

pagetabgridlayoutmanagerObj::implObj
::implObj(const pagetabgridcontainer_impl &my_container,
	  const ref<containerObj::implObj> &parent_container)

	: superclass_t{my_container},
	  my_container{my_container},
	  parent_container{parent_container}
{
}

pagetabgridlayoutmanagerObj::implObj::~implObj()=default;

layoutmanager pagetabgridlayoutmanagerObj::implObj::create_public_object()
{
	return pagetabgridlayoutmanager::create(ref(this));
}

void pagetabgridlayoutmanagerObj::implObj::recalculate(IN_THREAD_ONLY)
{
	superclass_t::recalculate(IN_THREAD);

	auto hv=container_impl->get_element_impl().get_horizvert(IN_THREAD);

	dim_t minimum_width=0;

	for (const auto &m:grid_elements(IN_THREAD)->horiz_metrics)
	{
		auto this_minimum=m.second.minimum();

		if (this_minimum > minimum_width)
			minimum_width=this_minimum;
	}

	auto vert_metrics=hv->vert;

	if (minimum_width == 0)
		vert_metrics={0, 0, 0};

	auto preferred=hv->horiz.preferred();

	if (preferred < minimum_width)
		preferred=minimum_width;

	parent_container->get_element_impl().get_horizvert(IN_THREAD)
		->set_element_metrics(IN_THREAD,
				      metrics::axis{minimum_width, preferred,
					dim_t::infinite()}, vert_metrics);
}


LIBCXXW_NAMESPACE_END
