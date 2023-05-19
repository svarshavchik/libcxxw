/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_layoutmanager_h
#define x_w_impl_layoutmanager_h

#include <x/w/layoutmanager.H>
#include <x/w/containerobj.H>
#include <x/w/rectangle.H>
#include <x/w/batch_queuefwd.H>
#include <x/w/rectanglefwd.H>
#include <x/w/impl/draw_infofwd.H>
#include <x/w/impl/scratch_buffer_drawfwd.H>
#include <x/w/impl/child_elementfwd.H>
#include <x/w/impl/element.H>
#include <x/w/connection_threadfwd.H>

#include <x/functionalrefptrfwd.H>
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

class layoutmanagerObj::implObj : virtual public obj {

 protected:
	LOG_CLASS_SCOPE;

 public:

	//! My container implementation.

	const container_impl layout_container_impl;

	//! Constructor
	implObj(const container_impl &layout_container_impl);

	//! Destructor
	~implObj();

	//! This layout manager is about to be uninstalled from its container.

	//! The default implementation does nothing.
	virtual void uninstalling(ONLY IN_THREAD);

	//! An widget in the container has updated metrics.

	//! The default implementation invokes needs_recalculation().

	virtual void child_metrics_updated(ONLY IN_THREAD);

	//! Mark this layout as needing recalculation.

	void needs_recalculation();

	//! Mark this layout as needing recalculation.

	void needs_recalculation(const batch_queue &queue);

	//! Mark this layout as needing recalculation.

	virtual void needs_recalculation(ONLY IN_THREAD);

	//! Iterate over all child elements

	template<typename functor_type>
		void for_each_child(ONLY IN_THREAD,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Subclass iterates over each child widget.
	virtual void do_for_each_child(ONLY IN_THREAD,
				       const function<void
				       (const element &e)> &)=0;

	//! The layout manager should know how many children it has.

	//! This is mostly for estimation purposes. The number of child
	//! elements can change at any time, so layout managers that have
	//! at most one child element, and usually do, are free to return 1
	//! all the time. This is used to reserve() a vector that might need
	//! to deal with all the children, and for_each_child() gives an
	//! authoritative answer.
	virtual size_t num_children(ONLY IN_THREAD)=0;

	//! Create a public object for this layout manager

	//! This is invoked by the container's get_layoutmanager().

	virtual layoutmanager create_public_object()=0;

	//! Implemented by subclass.

	//! This is called as a result of the public layoutmanager object
	//! getting destroyed. The layout manager can buffer changes to the
	//! container, and then process them in recalculate().
	//!
	//! If there are any new elements in the container, recalculate()
	//! should call initialize_if_needed() on them.

	virtual void recalculate(ONLY IN_THREAD)=0;

	//! The container's position has been updated.

	//! The container should, if necessary, recalculate and update
	//! the child elements' positions.

	virtual void process_updated_position(ONLY IN_THREAD,
					      const rectangle &position)=0;

	//! The container's position has not been changed.

	//! But the container can, if necessary, recalculate and update
	//! the child elements' positions.

	virtual void process_same_position(ONLY IN_THREAD,
					   const rectangle &position);

	//! A child element's background color has changed

	//! The default implementation does nothing
	virtual void child_background_color_changed(ONLY IN_THREAD,
						    const element_impl &child);

	//! A child element's visibility will change soon.

	//! The default implementation does nothing
	virtual void requested_child_visibility_changed
		(ONLY IN_THREAD,
		 const element_impl &child, bool);

	//! A child element's inherited_visibility has changed

	//! The default implementation does nothing
	virtual void inherited_child_visibility_changed
		(ONLY IN_THREAD,
		 const element_impl &child,
		 inherited_visibility_info &info);

	//! Return the child widget position that includes any container-provided padding.

	//! The default implementation returns the same rectangle as the
	//! element's current_position.

	virtual rectangle padded_position(ONLY IN_THREAD, const element_impl &e);

	//! Return this layout manager's widget.

	//! This allows mixin templates to be added to a subclass of either
	//! elementObj::implObj, or layoutmanagerObj::implObj

	elementObj::implObj &get_element_impl();

	//! Initialize the layout manager, in the connection thread.
	virtual void initialize(ONLY IN_THREAD);

	//! The theme has changed

	virtual void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);

	//! This widget wants to make sure that part of it is visible.

	//! An element's ensure_visibility() winds up here.
	//!
	//! The default implementation does nothing.
	//! The peephole layout manager makes sure that its widget scrolls
	//! this part of it into view.

	virtual void ensure_visibility(ONLY IN_THREAD,
				       elementObj::implObj &e,
				       const element_visibility_t &v);

	//! Invoke request_visibility_recursive() for each child.

	virtual void request_visibility_recursive(ONLY IN_THREAD, bool);

	//! All positions have been finalized.

	//! The default implementation does nothing.

	virtual void process_finalized_position(ONLY IN_THREAD);

	//! Invoked to draw the contents of the container.

	//! The default implementation uses container_clear_padding() to clear
	//! the padding around each widget. The passed-in drawn_areas
	//! gets forwarded to container_clear_padding(), in this manner it
	//! is populated with the areas occupied by all the elements in the
	//! container.
	virtual void do_draw(ONLY IN_THREAD,
			     const draw_info &di,
			     clip_region_set &clip,
			     rectarea &drawn_areas);

	//! Convenient shortcut
	void run_as(const functionref<void (ONLY IN_THREAD)> &f);

	//! Save this container's current state, if supported.

	//! The default implementation does nothing.
	virtual void save(ONLY IN_THREAD);
};

#define x_w_impl_layoutmanager_H_defined 1

#include <x/w/impl/container_layoutmanager.H>

LIBCXXW_NAMESPACE_END

#endif
