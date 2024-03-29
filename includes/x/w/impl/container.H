/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_container_H
#define x_w_impl_container_H

#include <x/w/elementobj.H>
#include <x/w/containerobj.H>
#include <x/w/layoutmanagerobj.H>
#include <x/w/rectangle.H>
#include <x/w/connection_threadfwd.H>
#include <x/w/defaultthemefwd.H>
#include <x/w/screen_positionsfwd.H>
#include <x/w/impl/draw_infofwd.H>
#include <x/w/impl/inherited_visibility_infofwd.H>
#include <x/w/impl/get_window_handler.H>
#include <x/w/impl/redraw_priority.H>
#include <x/w/impl/element_visibility.H>
#include <x/mpobj.H>
#include <x/functional.H>

#include <map>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! The container implementation object.

//! The container implementation object has an indirect link to the
//! container's layout manager's implementaiton object. The link is
//! installed and torn down by the container's public object, see
//! layoutmanagerObj::implObj.

class containerObj::implObj : public get_window_handlerObj,
			      virtual public obj {

public:

	//! Constructor

	implObj(get_window_handlerObj &parent_get_window_handler);

	//! Destructor

	~implObj();

	//! This container must be an widget.

	//! Must be implement in a subclass

	virtual elementObj::implObj &container_element_impl()=0;

	//! This container must be an widget.

	//! Must be implement in a subclass

	virtual const elementObj::implObj &container_element_impl() const=0;

	//! The mutex-protected container for the layout manager implementation.

	//! \see layoutmanagerObj::implObj

	typedef mpobj<layout_implptr> layoutmanager_ptr_t;

private:
	//! The link to the container's layout manager.

	mutable layoutmanager_ptr_t layoutmanager_ptr;
public:

	template<typename functor_type>
	void invoke_layoutmanager(functor_type &&functor);

	template<typename functor_type>
	void invoke_layoutmanager(functor_type &&functor) const;

	//! Invoked from layoutmanager's public object's constructor

	void install_layoutmanager(const layout_impl &) LIBCXX_HIDDEN;

	//! Invoked from layoutmanager's public object's destructor

	void uninstall_layoutmanager(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! If this widget is visible, recursively invoke draw_after_visibility_updated().

	//! Overridden in generic_window_handler. Top level visibility is
	//! translated into MapWindow(), and we'll draw everything when we
	//! get the exposure event.

	virtual void draw_child_elements_after_visibility_updated
		(ONLY IN_THREAD, bool flag);

	//! A child widget will be visible or not visible.

	void requested_child_visibility_updated(ONLY IN_THREAD,
						const element_impl &,
						bool);

	//! A child widget became visible or not visible.

	void inherited_child_visibility_updated(ONLY IN_THREAD,
						const element_impl &,
						inherited_visibility_info &);

	//! Add this container to containers_2_recalculate

	void needs_recalculation(ONLY IN_THREAD);

	//! Invoke the layout manager's needs_recalculation().

	//! Invokes the layout manager's virtual needs_recalculation(),
	//! whose default implementation simply calls needs_recalculation()
	//! here, but is also overridden by some layout managers.

	void tell_layout_manager_it_needs_recalculation(ONLY IN_THREAD);

	//! Draw the container.

	//! Sets up the clipping, and calls the layout manager's do_draw().
	//! Whatever the layout manager indicates is off limits, because it's
	//! occupied by child elements, or padding around them, will be
	//! clear_to_color().

	void do_draw(ONLY IN_THREAD,
		     const draw_info &di,
		     const rectarea &areas);

	//! Return the container's redraw priority

	//! The default implementation returns clear_area, the container
	//! itself only clears the areas not used by any child widgets.
	redraw_priority_t get_redraw_priority(ONLY IN_THREAD);

	//! Forcibly redraw the entire container.

	//! Invoked by layout managers in order to clear the filler space
	//! in the container, after they reposition their widgets.
	void do_draw(ONLY IN_THREAD);

	//! Container's inherited visibility has been updated, part 1.

	//! What needs to be done before container_element's overriden
	//! inherited_visibility_updated() forwards the overridden call
	//! to the superclass.
	//!
	//! If the container is being hidden, the child elements get
	//! get hidden first, and this propagates the inherited_visibility
	//! updated() call to them before this element's.

	void inherited_visibility_updated_before(ONLY IN_THREAD,
						 inherited_visibility_info &)
		LIBCXX_PUBLIC;

	//! Container's inherited visibility has been updated, part 2.

	//! What needs to be done after container_element's overriden
	//! inherited_visibility_updated() forwards the overridden call
	//! to the superclass.
	//!
	//! If the container is being shown, the child elements get
	//! get shown after propagating the inherited_visibility
	//! updated() call to the container widget.
	//!
	//! Also invokes the layout manager's inherited_visibility_updated().

	void inherited_visibility_updated_after(ONLY IN_THREAD,
						inherited_visibility_info &)
		LIBCXX_PUBLIC;

	//! The container's position has been updated.

	//! Invokes the layout manager's process_updated_position() callback.

	void process_updated_position(ONLY IN_THREAD);

	//! The container's position has not been updated.

	//! Breaking news? No, not really. But the grid layout manager may
	//! want to rethink its child elements' positions.

	void process_same_position(ONLY IN_THREAD);

	//! request_visibility_recursive() for all elements in the container.

	//! Recursively invoke the child elements'
	//! request_visibility_recursive.

	void request_child_visibility_recursive(ONLY IN_THREAD, bool flag);

	//! Invoke draw_child_elements_after_visibility_updated.

	void draw_after_visibility_updated(ONLY IN_THREAD, bool flag);

	//! Invoke process_finalized_position.

	void process_finalized_position(ONLY IN_THREAD);

	//! Invoke layout manager's initialize() method.

	void initialize(ONLY IN_THREAD);

	//! Invoke layout manager's theme_updated() method.

	void theme_updated(ONLY IN_THREAD, const const_defaulttheme &new_theme);
	//////////////////////////////////////////////////////////////////////

	//! Invoke a lambda for each child widget.

	template<typename functor_type>
		void for_each_child(ONLY IN_THREAD,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Iterate over all child elements.

	//! Invoke the layout manager's do_for_each_child.

	void do_for_each_child(ONLY IN_THREAD,
			       const function<void (const element &)> &);

	//! Return number of child elements.

	//! Invoke the layout manager's num_children.

	size_t num_children(ONLY IN_THREAD);

	//! Child element's background color has changed

	void child_background_color_changed(ONLY IN_THREAD,
					    const element_impl &child);

 private:
	void propagate_inherited_visibility(ONLY IN_THREAD,
					    inherited_visibility_info &info);

 public:

	//! This widget wants to make sure that this part of it is visible.

	//! Notifies the layout manager, then invokes
	//! ensured_visibility_of_child_element().

	void ensure_visibility(ONLY IN_THREAD, elementObj::implObj &e,
			       const element_visibility_t &v);

	//! This container itself was removed from its container.

	void removed_from_container(ONLY IN_THREAD);

	//! A child widget was just ensure_visibility()-ed.

	//! Overriden by the container_visible_element mixin.
	//!
	//! The default implementation checks the child element's current
	//! coordinates, then computes the corresponding coordinates in the
	//! container, then invokes the container's ensure_visibility().

	virtual void ensured_visibility_of_child_element(
		ONLY IN_THREAD,
		elementObj::implObj &e,
		const element_visibility_t &v);

	//! Forward save() to the layout manager.
	void save(ONLY IN_THREAD);
};

// The connection thread maintains a map of all containers that need to
// be recalculated. The map key is the nesting_level, the map value is a
// set of all containers, with that nesting level, that may need to be
// recalculated

class containers_2_recalculate_map
	: public std::map<size_t,
			  std::unordered_set<container_impl>>
{
};

#define x_w_impl_container_H_defined 1

#include <x/w/impl/container_layoutmanager.H>

LIBCXXW_NAMESPACE_END

#endif
