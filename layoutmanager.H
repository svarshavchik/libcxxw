/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef layoutmanager_h
#define layoutmanager_h

#include "x/w/layoutmanager.H"
#include "x/w/containerobj.H"
#include "x/w/rectangle.H"
#include "x/w/batch_queuefwd.H"
#include "draw_infofwd.H"
#include "child_elementfwd.H"
#include "element.H"
#include "connection_threadfwd.H"

#include <x/functional.H>
#include <x/logger.H>

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

 protected:
	LOG_CLASS_SCOPE;

 private:
	//! Whether this layout needs to be "recalculated"

	bool recalculate_needed_thread_only=false;

 public:

	THREAD_DATA_ONLY(recalculate_needed);

	//! My container implementation.

	const ref<containerObj::implObj> container_impl;

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();

	//! Mark this layout as needing recalculation.

	void needs_recalculation(const batch_queue &queue);

	//! Mark this layout as needing recalculation.

	void needs_recalculation(IN_THREAD_ONLY);

	//! Invoked by the connection thread. Checks recalculate_needed, invokes recalculate()

	void check_if_recalculate_needed(IN_THREAD_ONLY);

	//! Draw all elements in the container.

	//! Returns the drawn areas.

	virtual rectangle_set draw(IN_THREAD_ONLY,
				   const draw_info &di,
				   const rectangle_set &areas);

	//! Iterate over all child elements

	template<typename functor_type>
		void for_each_child(IN_THREAD_ONLY,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const child_element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Create a public object for this layout manager

	//! This is invoked by the container's get_layoutmanager().

	virtual layoutmanager create_public_object()=0;

 private:

	//! Implemented by subclass

	virtual void recalculate(IN_THREAD_ONLY)=0;

	//! Subclass iterates over each child element.
	virtual void do_for_each_child(IN_THREAD_ONLY,
				       const function<void
				       (const child_element &e)> &)=0;
};

LIBCXXW_NAMESPACE_END

#endif
