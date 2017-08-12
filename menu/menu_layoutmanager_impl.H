/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menu_layoutmanager_impl_h
#define menu_layoutmanager_impl_h

#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN menu_layoutmanager_implObj
	: public peepholed_toplevel_listcontainer_layoutmanager_implObj {


 public:

	menu_layoutmanager_implObj
		(const ref<peepholed_toplevel_listcontainer_implObj
		 > &container_impl,
		 const ref<listcontainerObj::implObj> &listcontainer_impl,
		 const new_listlayoutmanager &style);

	~menu_layoutmanager_implObj();
};

LIBCXXW_NAMESPACE_END

#endif
