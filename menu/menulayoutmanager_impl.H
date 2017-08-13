/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menu_layoutmanager_impl_h
#define menu_layoutmanager_impl_h

#include "peepholed_toplevel_listcontainer/layoutmanager_impl.H"
#include "x/w/menulayoutmanagerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see menulayoutmanager

class LIBCXX_HIDDEN menulayoutmanagerObj::implObj
	: public peepholed_toplevel_listcontainer_layoutmanager_implObj {


 public:

	//! Constructor
	implObj(const ref<peepholed_toplevel_listcontainer_implObj>
		&container_impl,
		const ref<listcontainerObj::implObj> &listcontainer_impl,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Create a menulayoutmanager object.

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
