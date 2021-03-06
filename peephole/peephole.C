/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "peephole/peephole_impl.H"
#include "peephole/peephole_layoutmanager_impl.H"
#include "peephole/peephole.H"
#include "x/w/element.H"
#include "x/w/impl/container.H"

LIBCXXW_NAMESPACE_START

peepholeObj::peepholeObj(const ref<implObj> &impl,
			 const ref<peepholelayoutmanagerObj::implObj> &layout)
	: containerObj{ref{&impl->get_container_impl()}, layout}, impl{impl},
	  peepholed_element
	{
	 layout->element_in_peephole->get_peepholed_element()
	}
{
}

peepholeObj::~peepholeObj()=default;


LIBCXXW_NAMESPACE_END
