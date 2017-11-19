/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_switchlayoutmanagerobj_h
#define x_w_switchlayoutmanagerobj_h

#include <x/w/switchlayoutmanagerfwd.H>
#include <x/w/switchfactoryfwd.H>
#include <x/w/elementfwd.H>
#include <x/w/layoutmanagerobj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see switchlayoutmanager

class switchlayoutmanagerObj : public layoutmanagerObj {

public:
	class implObj;

	//! Internal implementation object.
	const ref<implObj> impl;

	//! Constructor
	switchlayoutmanagerObj(const ref<implObj> &impl) LIBCXX_HIDDEN;

	//! Destructor
	~switchlayoutmanagerObj() LIBCXX_HIDDEN;

	//! Append new elements
	switchfactory append();

	//! Insert elements before an existing one.
	switchfactory insert(size_t);

	//! Remove an element.
	void remove(size_t);

	//! How many elements there are in the list.
	size_t size() const;

	//! Return the specified element.
	element get(size_t) const;

	//! Return the currently switched-to element, if there is one.

	std::optional<size_t> switched() const;

	//! Switch to the given element.
	void switch_to(size_t);

	//! Un-switch, don't show any element.
	void switch_off();
};

LIBCXXW_NAMESPACE_END

#endif
