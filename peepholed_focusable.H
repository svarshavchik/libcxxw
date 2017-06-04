/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef peepholed_focusable_h
#define peepholed_focusable_h

#include "peepholed_focusablefwd.H"
#include "peepholed_focusableobj.H"
#include "x/w/scrollbar.H"
#include "x/w/containerobj.H"
#include "x/w/gridlayoutmanagerfwd.H"
#include "scrollbar/scrollbarfwd.H"
#include "peephole/peepholefwd.H"
#include "peephole/peepholedfwd.H"
#include "peephole/peepholeobj.H"
#include "background_colorfwd.H"
#include "focus/focusablefwd.H"
#include <x/functional.H>
#include <x/ref.H>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Factory signature that gets passed to \ref create_peepholed_focusable_with_frame "create_peepholed_focusable_with_frame()".

typedef std::tuple<ref<peepholeObj::implObj>, peepholed,
		   focusable_impl> make_peepholed_func_t(const
							 ref<containerObj
							 ::implObj> &);

//! Type-erased implementation of \ref create_peepholed_focusable_with_frame "create_peepholed_focusable_with_frame()".

std::tuple<scrollbar, scrollbar, gridlayoutmanager>
create_peepholed_focusable_with_frame_impl(const char *border,
			       const char *inputfocusoff_border,
			       const char *inputfocuson_border,
			       double focusable_padding,
			       const background_color &focusable_background_color,
			       const ref<containerObj::implObj>
			       &parent_container,
			       const function<make_peepholed_func_t>
			       &make_peepholed,
			       scrollbar_visibility horizontal_visibility,
			       scrollbar_visibility vertical_visibility)
LIBCXX_HIDDEN;

//! Construct a focusframe with a peephole inside it, containing a focusable element.

//! Creates a scrollable peephole for a focusable element. The peephole has
//! focus frame around it.
//!
//! The display element must be a container, and the container's implementation
//! object gets created first.
//!
//! create_peepholed_focusable_with_frame() returns the peephole's vertical scrollbar,
//! the horizontal scrollbar, and the grid layout manager for the display
//! element. The grid layout manager should then be used to construct
//! the public display element's container superclass.
//!
//! The \c make_peepholed parameter is a lambda that gets invoked with a
//! container implementation object as its parameter. This will be the
//! focusframe container.
//!
//! The lambda must create a subclass of peepholeObj::implObj, using the
//! container implementation parameter to construct it. The lambda returns
//! a subclass of peepholeObj::implObj, the peepholed display element and its
//! focusable implementation object.
//!
//! The returned scrollbars, and the focusable implementation object should
//! be used to construct a \ref peepholed_focusable "peepholed_focusable".
//!
//! The public display element inherits from focusableObj, and delegates the
//! inherited methods to the peepholed_focusable, so it looks like a single
//! focusable display element.
//!
//! The focusable display element in the peephole also inherits from
//! focusableObj. The focus frame container registers itself as the owner of
//! the focusable object in the peephole.

template<typename make_peepholed_t>
inline std::tuple<scrollbar, scrollbar, gridlayoutmanager>
create_peepholed_focusable_with_frame
(//! The border around the focus frame and scrollbars.
 const char *border,
 //! Focus off border
 const char *inputfocusoff_border,
 //! Focus on border
 const char *inputfocuson_border,
 //! Padding between the scrollbar and the peephole.
 double focusable_padding,

 //! Focusable's background color.

 //! This actually sets the background color for
 //! the focus frame, with the peephole and the
 //! the focusable element inheriting it.
 const background_color &focusable_background_color,

 //! The implementation object of the display element under construction.
 const ref<containerObj::implObj>
 &parent_container,

 //! Factor for the peephole and the focusable.
 make_peepholed_t &&make_peepholed,

 //! Initial scrollbar visibility.
 scrollbar_visibility horizontal_visibility,

 //! Initial scrollbar visibility.
 scrollbar_visibility vertical_visibility)
{
	return create_peepholed_focusable_with_frame_impl
		(border,
		 inputfocusoff_border,
		 inputfocuson_border,
		 focusable_padding,
		 focusable_background_color,
		 parent_container,
		 make_function<make_peepholed_func_t>
		 (std::forward<make_peepholed_t>(make_peepholed)),
		 horizontal_visibility,
		 vertical_visibility);
}

LIBCXXW_NAMESPACE_END

#endif
