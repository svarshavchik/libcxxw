/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef listcontainer_impl_h
#define listcontainer_impl_h

#include "listcontainer.H"
#include "child_element.H"
#include "container_elementfwd.H"
#include "themedim_elementfwd.H"
#include "focus/focusable_elementfwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include "focus/focusable.H"

LIBCXXW_NAMESPACE_START

struct listcontainer_dim_v;
struct listcontainer_dim_left;
struct listcontainer_dim_inner;
struct listcontainer_dim_right;

//! Superclass of the list container implementation object

//! The list container implementation object stores a mixin with four
//! theme-based dimensions which set the list items' style.

typedef themedim_elementObj<focusable_elementObj
			    <container_elementObj<child_elementObj>>,
			    listcontainer_dim_v,
			    listcontainer_dim_left,
			    listcontainer_dim_inner,
			    listcontainer_dim_right
			    > listcontainer_impl_superclass_t;

//! List container implementation object.

//! The implementation object for a container that uses the
//! \ref listlayoutmanager "list layout manager".

class LIBCXX_HIDDEN listcontainerObj::implObj :
	public listcontainer_impl_superclass_t {

 public:

	//! Constructor

	implObj(const ref<containerObj::implObj> &parent,
		const new_listlayoutmanager &style);

	//! Destructor
	~implObj();

	//! Create an intermediate container for any new child element.

	//! When a new child element gets created for this container,
	//! its actual parent container is an intermediate container
	//! with a listitemlayoutmanager.
	//!
	//! The listlayoutmanager's factory collects each created element,
	//! and when there's enough of them for a new list item, they
	//! get added to the list layout manager.

	ref<containerObj::implObj>
		parent_for_new_child(const ref<containerObj::implObj> &parent)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
