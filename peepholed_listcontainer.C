/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"

#include "peepholed_listcontainer_impl.H"
#include "peepholed_listcontainer.H"
#include "reference_font_element.H"

LIBCXXW_NAMESPACE_START

peepholed_listcontainerObj
::peepholed_listcontainerObj(const ref<implObj> &impl,
			     const ref<listlayoutmanagerObj::implObj>
			     &list_impl)
	: listcontainerObj(impl, list_impl),
	  impl(impl)
{
}

peepholed_listcontainerObj::~peepholed_listcontainerObj()=default;

element peepholed_listcontainerObj::get_element()
{
	return element(this);
}

dim_t peepholed_listcontainerObj::horizontal_increment(IN_THREAD_ONLY) const
{
	return impl->reference_font_element<>::font_nominal_width(IN_THREAD);
}

dim_t peepholed_listcontainerObj::vertical_increment(IN_THREAD_ONLY) const
{
	return impl->rowsize(IN_THREAD);
}

LIBCXXW_NAMESPACE_END
