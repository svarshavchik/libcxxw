/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef container_H
#define container_H

#include "x/w/containerobj.H"
#include "x/w/layoutmanagerobj.H"
#include "child_element.H"

#include <x/mpobj.H>

#include <map>
#include <set>

LIBCXXW_NAMESPACE_START

//! The container implementation object.

//! The container implementation object has an indirect link to the
//! container's layout manager's implementaiton object. The link is
//! installed and torn down by the container's public object, see
//! layoutmanagerObj::implObj.

class LIBCXX_HIDDEN containerObj::implObj : public elementObj::implObj {

public:

	//! Constructor

	using elementObj::implObj::implObj;

	//! Destructor

	~implObj();

	//! The mutex-protected container for the layout manager implementation.

	//! \see layoutmanagerObj::implObj

	typedef mpobj<ptr<layoutmanagerObj::implObj>> layoutmanager_ptr_t;

	//! The link to the container's layout manager.

	layoutmanager_ptr_t layoutmanager_ptr;

	//! If the layout manager has been installed, invoke it.

	//! This nicely handles race conditions during destructions by
	//! doing nothing.

	template<typename functor_type>
		void invoke_layoutmanager(functor_type &&functor)
	{
		typedef typename layoutmanager_ptr_t::lock lock_t;

		ptr<layoutmanagerObj::implObj> p=
			*lock_t(layoutmanager_ptr);

		if (p.null())
			return; // In the process of being destroyed

		functor(ref<layoutmanagerObj::implObj>(p));
	}

	//! Invoked from layoutmanager's public object's constructor

	void install_layoutmanager(const ref<layoutmanagerObj::implObj> &);

	//! Invoked from layoutmanager's public object's destructor

	void uninstall_layoutmanager();
};

// The connection thread maintains a map of all containers that need to
// be recalculated. The map key is the nesting_level, the map value is a
// set of all containers, with that nesting level, that may need to be
// recalculated

class LIBCXX_HIDDEN containers_2_recalculate_map
	: public std::map<size_t,
			  std::set<ref<containerObj::implObj>>>
{
};

LIBCXXW_NAMESPACE_END

#endif
