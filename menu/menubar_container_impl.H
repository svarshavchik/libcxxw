/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menubar_container_impl_h
#define menubar_container_impl_h

#include "menu/menubar_container_implfwd.H"
#include "container_elementfwd.H"
#include "container.H"
#include "always_visiblefwd.H"
#include "child_element.H"
#include "x/w/menufwd.H"

LIBCXXW_NAMESPACE_START

//! Typedef alias for menubar_container_implObj's superclass.
typedef always_visibleObj<container_elementObj<child_elementObj>
			  > menubar_container_superclass_impl_t;

//! Menu bar container implementation object.

//! Not much added functionality.

class LIBCXX_HIDDEN menubar_container_implObj
	: public menubar_container_superclass_impl_t {

 public:

	//! Constructor
	menubar_container_implObj(const ref<containerObj::implObj> &parent);

	//! Destructor
	~menubar_container_implObj();

	//! Fix tabbing order of a newly-added element.

	//! Keep the tabbing order of the menu bar elements to be the first
	//! ones tabbed into.

	void fix_order(IN_THREAD_ONLY, const menu &new_element);
};

LIBCXXW_NAMESPACE_END

#endif