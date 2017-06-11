/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listlayoutmanager_h
#define listlayoutmanager_h

#include "x/w/listlayoutmanager.H"
#include "gridlayoutmanager.H"
#include "listlayoutmanager/listcontainer.H"
#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! The list layout manager implementation object.

//! The list layout manager is really the grid layout manager in disguise.
//!
//! \see listlayoutmanager

class LIBCXX_HIDDEN listlayoutmanagerObj::implObj
	: public gridlayoutmanagerObj::implObj {

 public:
	const ref<listcontainerObj::implObj> container_impl;

	//! This list layout manager's style.
	const listlayoutstyle &style;

	//! Number of columns
	const size_t columns;

	//! Constructor
	implObj(const ref<listcontainerObj::implObj> &container_impl,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Override create_public_object to create the listlayoutmanager.

	layoutmanager create_public_object() override;
};

LIBCXXW_NAMESPACE_END

#endif
