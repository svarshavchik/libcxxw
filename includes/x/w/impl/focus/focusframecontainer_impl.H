/*
** Copyright 2017-2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_focusframecontainer_impl_h
#define x_w_impl_focusframecontainer_impl_h

#include <x/w/containerobj.H>
#include <x/w/elementobj.H>
#include <x/w/focus.H>
#include <x/w/impl/current_border_implfwd.H>
#include <x/w/connection_threadfwd.H>

LIBCXXW_NAMESPACE_START

//! A mixin for a container that uses the \ref focusframelayoutimplObj "focus frame layout manager".

//! \see focusframecontainer_elementObj

class focusframecontainer_implObj : virtual public obj {

 public:

	//! Constructor
	focusframecontainer_implObj();

	//! Destructor
	~focusframecontainer_implObj();

	//! Implemented by the mixin template, to return my container.

	virtual containerObj::implObj &get_container_impl()=0;

	//! Implemented by the mixin template, to return my container.

	virtual const containerObj::implObj &get_container_impl() const=0;

	//! Implemented by the mixin template, to return my focusoff border.

	virtual current_border_impl get_focusoff_border() const=0;

	//! Implemented by the mixin template, to return my focuson border.

	virtual current_border_impl get_focuson_border() const=0;

	//! Trigger a container rebuild, in order to update the focus frame.

	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger);

	//! Trigger a container rebuild, in order to update the focus frame.

	void window_focus_change(ONLY IN_THREAD, bool flag);

 private:
	//! Rebuild the container
	void update_focusframe(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif