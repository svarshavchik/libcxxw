/*
** Copyright 2017-2018 Double Precision, Inc.
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
#include <x/w/impl/draw_infofwd.H>
#include <x/w/impl/inherited_visibility_infofwd.H>
#include <x/w/impl/get_window_handler.H>
#include <x/mpobj.H>
#include <x/functional.H>

#include <map>

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

	implObj();

	//! Destructor

	~implObj();

	//! This container must be an element.

	//! Must be implement in a subclass

	virtual elementObj::implObj &container_element_impl()=0;

	//! This container must be an element.

	//! Must be implement in a subclass

	virtual const elementObj::implObj &container_element_impl() const=0;

	//! Return my window
	generic_windowObj::handlerObj &get_window_handler() override;

	//! Return my window
	const generic_windowObj::handlerObj &get_window_handler() const
		override;

	//! The mutex-protected container for the layout manager implementation.

	//! \see layoutmanagerObj::implObj

	typedef mpobj<layout_implptr> layoutmanager_ptr_t;

	//! The link to the container's layout manager.

	layoutmanager_ptr_t layoutmanager_ptr;

	//! If the layout manager has been installed, invoke it.

	//! This nicely handles race conditions during destructions by
	//! doing nothing.

	template<typename functor_type>
		inline void invoke_layoutmanager(functor_type &&functor)
	{
		auto p=layoutmanager_ptr.get();

		if (!p)
			return; // In the process of being destroyed

		functor(layout_impl{p});
	}

	//! Invoked from layoutmanager's public object's constructor

	void install_layoutmanager(const layout_impl &) LIBCXX_HIDDEN;

	//! Invoked from layoutmanager's public object's destructor

	void uninstall_layoutmanager(ONLY IN_THREAD) LIBCXX_HIDDEN;

	//! If this element is visible, recursively invoke draw_after_visibility_updated().

	//! Overridden in generic_window_handler. Top level visibility is
	//! translated into MapWindow(), and we'll draw everything when we
	//! get the exposure event.

	virtual void draw_child_elements_after_visibility_updated
		(ONLY IN_THREAD, bool flag);

	//! A child element will be visible or not visible.

	void requested_child_visibility_updated(ONLY IN_THREAD,
						const element_impl &,
						bool);

	//! A child element became visible or not visible.

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
		     const rectangle_set &areas);

	//! Forcibly redraw the entire container.

	//! Invoked by layout managers in order to clear the filler space
	//! in the container, after they reposition their display elements.
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
	//! updated() call to the container element.
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

	//! Invoke layout manager's initialize() method.

	void initialize(ONLY IN_THREAD);

	//! Invoke layout manager's theme_updated() method.

	void theme_updated(ONLY IN_THREAD, const defaulttheme &new_theme);
	//////////////////////////////////////////////////////////////////////

	//! Invoke a lambda for each child element.

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

	//! Child element's background color has changed

	void child_background_color_changed(ONLY IN_THREAD,
					    const element_impl &child);

 private:
	void propagate_inherited_visibility(ONLY IN_THREAD,
					    inherited_visibility_info &info);

 public:

	//! This element wants to make sure that this part of it is visible.

	//! Notifies the layout manager, then invokes
	//! ensured_visibility_of_child_element().

	void ensure_visibility(ONLY IN_THREAD, elementObj::implObj &e,
			       const rectangle &r);

	//! This container itself was removed from its container.

	void removed_from_container(ONLY IN_THREAD);

	//! A child element was just ensure_visibility()-ed.

	//! Overriden by the container_visible_element mixin.
	//!
	//! The default implementation checks the child element's current
	//! coordinates, then computes the corresponding coordinates in the
	//! container, then invokes the container's ensure_visibility().

	virtual void ensured_visibility_of_child_element(ONLY IN_THREAD,
							 elementObj::implObj &e,
							 const rectangle &r);
};

// The connection thread maintains a map of all containers that need to
// be recalculated. The map key is the nesting_level, the map value is a
// set of all containers, with that nesting level, that may need to be
// recalculated

class LIBCXX_HIDDEN containers_2_recalculate_map
	: public std::map<size_t,
			  std::unordered_set<container_impl>>
{
};

LIBCXXW_NAMESPACE_END

#endif
