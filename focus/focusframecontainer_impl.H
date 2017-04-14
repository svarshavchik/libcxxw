/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef focusframecontainer_impl_h
#define focusframecontainer_impl_h

#include "x/w/containerobj.H"
#include "x/w/elementobj.H"
#include "focus/focus.H"
#include "focus/focusframecontainerobj.H"
#include "connection_threadfwd.H"

LIBCXXW_NAMESPACE_START

//! focusframecontainer implementation object.

class LIBCXX_HIDDEN focusframecontainerObj::implObj : virtual public obj {

 public:

	//! Constructor
	implObj();

	//! Destructor
	~implObj();

	//! Implemented by the mixin template, to return my container.

	virtual containerObj::implObj &get_container_impl()=0;

	//! Implemented by the mixin template, to return my container.

	virtual const containerObj::implObj &get_container_impl() const=0;

	//! Trigger a container rebuild, in order to update the focus frame.

	void keyboard_focus(IN_THREAD_ONLY,
			    focus_change event,
			    const ref<elementObj::implObj> &ptr);

	//! Trigger a container rebuild, in order to update the focus frame.

	void window_focus_change(IN_THREAD_ONLY, bool flag);

 private:
	//! Rebuild the container
	void update_focusframe(IN_THREAD_ONLY);
};

LIBCXXW_NAMESPACE_END

#endif
