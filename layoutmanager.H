/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef layoutmanager_H
#define layoutmanager_h

#include "x/w/layoutmanager.H"
#include "x/w/containerobj.H"
#include "element.H"

LIBCXXW_NAMESPACE_START

//! Layout manager implementation object.

//! A layout manager is always attached to a container.
//! The public container object has a reference to the public layoutmanager
//! object, that has a reference to this implementation object.
//!
//! The container implementation object has a mutex-protected nullable ptr to
//! this implementation object.
//!
//! This object contains references to the container implementation object
//! and to the elements in the container which, as all elements do,
//! have a reference to their parent object, the container object.
//!
//! These circular references are handled by the layout manager public object.
//!
//! Its constructor installs the reference to the implementation object into
//! the mutex-protected ptr in the container object. Its destructor removes
//! this reference.
//!
//! As such, the public object will break all circular references, when the
//! public layout manager object, that's referenced only from the public
//! container object, gets destroyed.

class LIBCXX_HIDDEN layoutmanagerObj::implObj : virtual public obj {

 public:

	//! My container implementation.

	const ref<containerObj::implObj> container_impl;

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();
};

LIBCXXW_NAMESPACE_END

#endif
