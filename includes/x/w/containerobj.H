/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_containerobj_h
#define x_w_containerobj_h

#include <x/w/containerfwd.H>
#include <x/w/layoutmanagerfwd.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/elementobj.H>

LIBCXXW_NAMESPACE_START

//! Methods common to all display containers.

//! \see container

class containerObj : public elementObj {

public:
	class LIBCXX_PUBLIC implObj;

	//! Internal implementation object.

	const ref<implObj> impl;

private:
	//! Layout manager's internal implementation object.

	const layout_impl container_layout_impl;
public:
	//! Return my implementation object.

	//! This is overridden by top level elements, like main_windowObj,
	//! which installs a grid layout manager instead of the requested
	//! one, and then creates an internal widget with the requested
	//! layout manager, then overrides get_layout_impl() so that it
	//! looks like the top level widget is using that layout
	//! manager.

	virtual layout_impl get_layout_impl() const;

	//! Return this container's layout manager.
	layoutmanager get_layoutmanager();

	//! Return this container's layout manager.
	const_layoutmanager get_layoutmanager() const;

	//! Constructor
	containerObj(const ref<implObj> &impl,
		     const layout_impl &container_layout_impl);

	//! Destructor
	~containerObj();
};

//! Alias
typedef ref<containerObj::implObj> container_impl;

//! Alias
typedef ptr<containerObj::implObj> container_implptr;

LIBCXXW_NAMESPACE_END

#endif
