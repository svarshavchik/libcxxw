/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef container_H
#define container_H

#include "x/w/containerobj.H"
#include "x/w/layoutmanagerobj.H"
#include "x/w/elementobj.H"
#include "x/w/generic_windowobj.H"
#include "x/w/rectangle.H"
#include "connection_threadfwd.H"
#include "draw_infofwd.H"
#include <x/mpobj.H>
#include <x/functional.H>

#include <map>
#include <set>

LIBCXXW_NAMESPACE_START

//! The container implementation object.

//! The container implementation object has an indirect link to the
//! container's layout manager's implementaiton object. The link is
//! installed and torn down by the container's public object, see
//! layoutmanagerObj::implObj.

class LIBCXX_HIDDEN containerObj::implObj : virtual public obj {

public:

	//! Constructor

	implObj();

	//! Destructor

	~implObj();

	//! This container must be an element.

	//! Must be implement in a subclass

	virtual elementObj::implObj &get_element_impl()=0;

	//! This container must be an element.

	//! Must be implement in a subclass

	virtual const elementObj::implObj &get_element_impl() const=0;

	//! Return my window
	virtual generic_windowObj::handlerObj &get_window_handler()=0;

	//! Return my window
	virtual const generic_windowObj::handlerObj &get_window_handler() const=0;

	//! The mutex-protected container for the layout manager implementation.

	//! \see layoutmanagerObj::implObj

	typedef mpobj<ptr<layoutmanagerObj::implObj>> layoutmanager_ptr_t;

	//! The link to the container's layout manager.

	layoutmanager_ptr_t layoutmanager_ptr;

	//! If the layout manager has been installed, invoke it.

	//! This nicely handles race conditions during destructions by
	//! doing nothing.

	template<typename functor_type>
		void invoke_layoutmanager(functor_type &&functor)
	{
		typedef typename layoutmanager_ptr_t::lock lock_t;

		ptr<layoutmanagerObj::implObj> p=
			*lock_t(layoutmanager_ptr);

		if (p.null())
			return; // In the process of being destroyed

		functor(ref<layoutmanagerObj::implObj>(p));
	}

	//! Invoked from layoutmanager's public object's constructor

	void install_layoutmanager(const ref<layoutmanagerObj::implObj> &);

	//! Invoked from layoutmanager's public object's destructor

	void uninstall_layoutmanager();

	//! If this element is visible, recursively invoke draw_after_visibility_updated().

	//! Overridden in generic_window_handler. Top level visibility is
	//! translated into MapWindow(), and we'll draw everything when we
	//! get the exposure event.

	virtual void draw_child_elements_after_visibility_updated
		(IN_THREAD_ONLY, bool flag);

	/////////////////////////////////////////////////////////////////////
	//
	// The parent class that inherits from container and element must
	// override the following element methods, and invoke the container's
	// methods.
	//
	// These methods are overridden using
	// container_element_overrides_decl.H and
	// container_element_overrides_impl.H.

	//! Draw the container.

	//! Punts to the layout manager, to draw all elements in the
	//! container, the fills in the remaining areas with the background
	//! color.

	void do_draw(IN_THREAD_ONLY,
		     const draw_info &di,
		     const rectangle_set &areas);

	//! Container's inherited visibility has been updated.

	//! The subclass must inherit from elementObj::implObj, and override
	//! inherited_visibility_updated, and invoke this method.

	void inherited_visibility_updated(IN_THREAD_ONLY,
					  bool flag);

	//! The container's position has been updated.

	//! Invokes the layout manager's process_updated_position() callback,
	//! then the container's element's notify_updated_position().

	void process_updated_position(IN_THREAD_ONLY);

	//! Recursive show() or hide().

	//! Recursively invoke the child elements'
	//! request_visibility_recursive in addition to invoking the
	//! container's element's request_visibility().

	void request_visibility_recursive(IN_THREAD_ONLY, bool flag);

	//! Invoke draw_child_elements_after_visibility_updated.

	void draw_after_visibility_updated(IN_THREAD_ONLY, bool flag);

	//////////////////////////////////////////////////////////////////////

	//! Invoke a lambda for each child element.

	template<typename functor_type>
		void for_each_child(IN_THREAD_ONLY,
				    functor_type &&functor)
	{
		do_for_each_child(IN_THREAD,
				  make_function<void (const element &e)>
				  (std::forward<functor_type>(functor)));
	}

	//! Iterate over all child elements.

	//! Invoke the layout manager's do_for_each_child.

	void do_for_each_child(IN_THREAD_ONLY,
			       const function<void (const element &)> &);

 private:
	void propagate_inherited_visibility(IN_THREAD_ONLY, bool flag);
};

// The connection thread maintains a map of all containers that need to
// be recalculated. The map key is the nesting_level, the map value is a
// set of all containers, with that nesting level, that may need to be
// recalculated

class LIBCXX_HIDDEN containers_2_recalculate_map
	: public std::map<size_t,
			  std::set<ref<containerObj::implObj>>>
{
};

LIBCXXW_NAMESPACE_END

#endif
