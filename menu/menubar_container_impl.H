/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef menubar_container_impl_h
#define menubar_container_impl_h

#include "menu/menubar_container_implfwd.H"
#include "x/w/impl/container_elementfwd.H"
#include "x/w/impl/container.H"
#include "x/w/impl/always_visiblefwd.H"
#include "x/w/impl/child_element.H"
#include "x/w/menufwd.H"

LIBCXXW_NAMESPACE_START

//! Typedef alias for menubar_container_implObj's superclass.
typedef always_visibleObj<container_elementObj<child_elementObj>,
			  false> menubar_container_superclass_impl_t;

//! Menu bar container implementation object.

//! Not much added functionality.

class LIBCXX_HIDDEN menubar_container_implObj
	: public menubar_container_superclass_impl_t {

 public:

	//! Constructor
	menubar_container_implObj(const container_impl &parent,
				  const color_arg &background_color);

	//! Destructor
	~menubar_container_implObj();

	//! Fix tabbing order of a newly-added element.

	//! Keep the tabbing order of the menu bar elements to be the first
	//! ones tabbed into.

	void fix_order(ONLY IN_THREAD, const menu &new_element);
};

LIBCXXW_NAMESPACE_END

#endif
