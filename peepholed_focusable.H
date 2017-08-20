/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_focusable_h
#define peepholed_focusable_h

#include "peepholed_focusable_impl.H"
#include "x/w/containerobj.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "x/w/scrollbarfwd.H"
#include "scrollbar/scrollbarfwd.H"
#include "peephole/peephole_style.H"
#include "peephole/peepholefwd.H"
#include "peephole/peepholedfwd.H"
#include "peephole/peepholeobj.H"
#include "background_colorfwd.H"
#include "focus/focusablefwd.H"
#include "nonrecursive_visibilityfwd.H"
#include "container_elementfwd.H"
#include "child_element.H"

#include <x/functional.H>
#include <x/ref.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Factory signature that gets passed to \ref create_peepholed_focusable_with_frame "create_peepholed_focusable_with_frame()".

typedef std::tuple<ref<peepholeObj::implObj>, peepholed,
		   focusable,
		   focusable_impl> make_peepholed_func_t(const
							 ref<containerObj
							 ::implObj> &);

//! Non type-erased parameters to create_peepholed_focusable_with_frame()

struct LIBCXX_HIDDEN create_peepholed_focusable_args_t {
	//! The border around the focus frame and scrollbars.
	const char *border;
	//! Focus off border
	const char *inputfocusoff_border;
	//! Focus on border
	const char *inputfocuson_border;
	//! Padding between the scrollbar and the peephole.
	double focusable_padding;

	//! Focusable's background color.

	//! This actually sets the background color for
	//! the focus frame, with the peephole and the
	//! the focusable element inheriting it.
	const background_color &focusable_background_color;

	//! The implementation object of the new display element under construction.

	//! This implementation object should inherit from the
	//! nonrecursive_visibility mixin.
	const ref<containerObj::implObj> &parent_container;

	//! Peephole's style
	peephole_style style;

	//! Initial scrollbar visibility.
	scrollbar_visibility horizontal_visibility;

	//! Initial scrollbar visibility.
	scrollbar_visibility vertical_visibility;
};

//! Type-erased implementation of \ref create_peepholed_focusable_with_frame "create_peepholed_focusable_with_frame()".

std::tuple<ref<peepholed_focusableObj::implObj>, gridlayoutmanager>
create_peepholed_focusable_with_frame_impl(const
					   create_peepholed_focusable_args_t &,
					   const function<make_peepholed_func_t>
					   &make_peepholed)
LIBCXX_HIDDEN;

//! The default parent_container for created_peepholed_focusable_with_frame

//! The internal implementation object for the peepholed focusable is a
//! container, and it should also inherit from the nonrecursive_visibilityObj
//! because every contained child element is visible, and the entire
//! display element is shown/hidden as a single element.

typedef nonrecursive_visibilityObj<container_elementObj<child_elementObj>
				  > peepholed_container_impl_t;

//! Construct a focusframe with a peephole inside it, containing a focusable element.

//! Creates a scrollable peephole for a focusable element. The peephole has
//! focus frame around it.
//!
//! The display element must be a container, and the container's implementation
//! object gets created first.
//!
//! create_peepholed_focusable_with_frame() returns the
//! \ref peepholed_focusable "peepholed_focusable"
//! implementation object
//! and the grid layout manager for the container's implementation
//! object.
//!
//! The public object should inherit from peepholed_focusableObj, passing
//! the \c peepholed_focusable implementation object together with the
//! layout manager to the constructor.
//!
//! The \c make_peepholed parameter is a lambda that gets invoked with a
//! container implementation object as its parameter. This will be the
//! focusframe container.
//!
//! The lambda must create a subclass of peepholeObj::implObj, using the
//! container implementation parameter to construct it. The lambda returns
//! a subclass of peepholeObj::implObj, the peepholed display element,
//! its focusable, and its focusable implementation object.
//!
//! The public display element inherits from focusableObj, and delegates the
//! inherited methods to the peepholed_focusable, so it looks like a single
//! focusable display element.
//!
//! The focusable display element in the peephole also inherits from
//! focusableObj. The focus frame container registers itself as the owner of
//! the focusable object in the peephole.

template<typename make_peepholed_t>
inline std::tuple<ref<peepholed_focusableObj::implObj>, gridlayoutmanager>
create_peepholed_focusable_with_frame
(const create_peepholed_focusable_args_t &args,
 //! Factor for the peephole and the focusable.
 make_peepholed_t &&make_peepholed)
{
	return create_peepholed_focusable_with_frame_impl
		(args,
		 make_function<make_peepholed_func_t>
		 (std::forward<make_peepholed_t>(make_peepholed)));
}

LIBCXXW_NAMESPACE_END

#endif
