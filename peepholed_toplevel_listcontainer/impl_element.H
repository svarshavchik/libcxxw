/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_toplevel_listcontainer_impl_element_h
#define peepholed_toplevel_listcontainer_impl_element_h

#include "peepholed_toplevel_listcontainer/impl_elementfwd.H"
#include "listlayoutmanager/listcontainer_impl.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
peepholed_toplevel_listcontainer_impl_elementObj<element_impl>
::~peepholed_toplevel_listcontainer_impl_elementObj()=default;

template<typename element_impl>
listcontainerObj::implObj
&peepholed_toplevel_listcontainer_impl_elementObj<element_impl>
::listcontainer_element()
{
	return *this;
}

LIBCXXW_NAMESPACE_END

#endif