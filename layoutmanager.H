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

 public:

	//! My container implementation.

	const ref<containerObj::implObj> container_impl;

	//! Constructor
	implObj(const ref<containerObj::implObj> &container_impl);

	//! Destructor
	~implObj();

	//! An element in the container has updated metrics.

	//! The default implementation invokes needs_recalculation().

	virtual void child_metrics_updated(IN_THREAD_ONLY);

	//! Mark this layout as needing recalculation.

	void needs_recalculation(const batch_queue &queue);

	//! Mark this layout as needing recalculation.

	void needs_recalculation(IN_THREAD_ONLY);

	//! Iterate over all child elements

	template<typename functor_type>
		void for_each_child(IN_THREAD_ONLY,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Subclass iterates over each child element.
	virtual void do_for_each_child(IN_THREAD_ONLY,
				       const function<void
				       (const element &e)> &)=0;

	//! Create a public object for this layout manager

	//! This is invoked by the container's get_layoutmanager().

	virtual layoutmanager create_public_object()=0;

	//! Implemented by subclass

	virtual void recalculate(IN_THREAD_ONLY)=0;

	//! The container's position has been updated.

	virtual void process_updated_position(IN_THREAD_ONLY,
					      const rectangle &position)=0;
 protected:

	//! Invokes the container's element function of the same name.

	void current_position_updated(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
