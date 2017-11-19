/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_switchlayoutmanager_h
#define x_w_switchlayoutmanager_h

#include <x/w/switchlayoutmanagerfwd.H>
#include <x/w/switchlayoutmanagerobj.H>
#include <x/w/layoutmanager.H>
#include <x/w/new_layoutmanagerfwd.H>

LIBCXXW_NAMESPACE_START

//! A lock on the contents of the \ref switchlayoutmanager "switch layout manager".

//! Blocks other execution threads from accessing the contents of the
//! switch layout manager.

class LIBCXX_PUBLIC switch_lock {

	//! Internal lock.
	mpobj_lock<switch_layout_info_s, std::recursive_mutex> lock;

 public:

	//! Constructor
	switch_lock(const switchlayoutmanager &);

	//! Destructor
	~switch_lock();
};

//! Create a new container that uses the \ref switchlayoutmanager "switch layout manager"

class LIBCXX_HIDDEN new_switchlayoutmanager : public new_layoutmanager {


public:
	//! Constructor
	new_switchlayoutmanager() LIBCXX_PUBLIC;

	//! Destructor
	~new_switchlayoutmanager() LIBCXX_PUBLIC;

	//! Create a container with the switch layout manager.

	//! \internal

	ref<layoutmanagerObj::implObj> create(const ref<containerObj::implObj> &
					      ) const override;
};

LIBCXXW_NAMESPACE_END

#endif
