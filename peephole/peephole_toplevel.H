/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef peephole_toplevel_H
#define peephole_toplevel_H

#include "x/w/container.H"
#include "x/w/layoutmanagerfwd.H"
#include "peephole/peepholed_toplevelfwd.H"

#include <x/functional.H>

LIBCXXW_NAMESPACE_START

//! A factory for create_peephole_toplevel().

typedef peepholed_toplevel
create_peepholed_element_t(const ref<containerObj::implObj> &);

//! Type-erased implementation of create_peephole_toplevel().

layoutmanager
create_peephole_toplevel_impl(const ref<containerObj::implObj> &toplevel,
			      const function<create_peepholed_element_t>
			      &factory) LIBCXX_HIDDEN;

template<typename create_peepholed_element_factory_t>
auto create_peephole_toplevel(const ref<containerObj::implObj> &toplevel,
			      create_peepholed_element_factory_t
			      &&element_factory)
	LIBCXX_HIDDEN;

//! Create a peephole for the top level display element.

//! The top level display element's peephole results in the top level
//! display element showing scrollbars if the top level display element's
//! size exceeds the maximum screen size.
//!
//! The top level container's implementation object, a containerObj::implObj
//! gets passed as the first parameter.
//!
//! The second parameter is a factory for the peepholed element.
//! The factory receives a container implementation object, and creates
//! a child element of the container, that also implements the peepholed
//! mixin (a subclass of peepholed_toplevel_elementObj).
//!
//! The implementation object for the top level display element that's
//! returned by the factory should implement the always_visible mixin,
//! and the factory should show() it, before the factory returns the
//! peepholed element.
//!
//! A top level display element gets laid out as follows:
//!
//! create_peephole_toplevel() creates its own layout manager for the
//! top level display element (the grid layout manager, that positions
//! the peephole and its scrollbars).
//!
//! create_peephole_toplevel() creates a peephole element, that goes into
//! the grid layout manager together with its scrollbars), then calls the
//! supplied element_factory, passing to it the peephole as its parent
//! implementation object.
//!
//! create_peephole_toplevel() returns the grid layout manager it created.
//! This layout manager should then be used to construct the public interface
//! object for the top level display element, which is a container, with the
//! constructed layout manager initializing the containerObj.

template<typename create_peepholed_element_factory_t>
auto create_peephole_toplevel(const ref<containerObj::implObj> &toplevel,
			      create_peepholed_element_factory_t
			      &&element_factory)
{
	return create_peephole_toplevel_impl
		(toplevel,
		 make_function<create_peepholed_element_t>
		 (std::forward<create_peepholed_element_factory_t>
		  (element_factory)));
}

LIBCXXW_NAMESPACE_END

#endif