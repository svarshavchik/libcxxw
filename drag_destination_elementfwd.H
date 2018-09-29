/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drag_destination_elementfwd_H
#define drag_destination_elementfwd_H

#include "x/w/impl/selection/current_selection_handlerfwd.H"
#include "x/w/elementobj.H"
#include "x/w/impl/dnd.H"

LIBCXXW_NAMESPACE_START

template<typename element_impl>
class LIBCXX_HIDDEN drag_destination_elementObj;

//! A mixin for a display element that accepts drag-and-dropped content.

//! The template parameter must be a subclass of child_elementObj, and the
//! template inherits from this class.
//!
//! The display element implements accepts_drop(), and drop(), and optionally
//! implements dragging_location(). This template defines accept_drop() and
//! the other two methods are inherited from elementObj::implObj.
//!
//! accepts_drop() receives the list of available drag and drop format types,
//! and returns true if the display element accepts any of the source formats
//! are acceptable. If accepts_drop() returns true, dragging_location()
//! gets called indicating the location in the display element where the
//! pointer is dragging.
//!
//! drop() gets called to begin a drop. drop() must return
//! a \ref current_selection_handler "current_selection_handler" for the
//! dragged selection, and set its \c type parameter to the format of the
//! converted selection. It should be one of the types that was passed to
//! accepts_drop().
//!
//! drop() also receives a mcguffin parameter. The mcguffin must continue to
//! exist until the entire dragged element selection has been received.
//! Passing the mcguffin to
//! \ref current_selection_paste_handler "current_selection_paste_handler"
//! is one valid approach. The returned selection handler get installed, and
//! remains in existence until the converted selection is fully processed,
//! and dropping the mcguffin into the returned
//! \c current_selection_paste_handler is one way to do it.
//!
//! The constructor parameters get forwarded to the superclass.
//!
//! The display element also implements show_droppable_pointer() and
//! show_notdroppable_pointer(), in order to install a cursor pointer image
//! to provide feedback whether the content is droppable, or not.
//!
//! \para Proxies
//!
//! generic_window_handlerObj calls find_acceptable_drop() for the display
//! element under the pointer. If the element does not accept the drop,
//! the call gets forwarded to its container, then its's container, and so on,
//! until the entire hierarchy of elements and containers under the pointer
//! is checked.
//!
//! A container that knows that one of its elements accepts dropped selections
//! can forward find_acceptable_drop() to it by invoking its
//! this_element_accepts_this_drop(), which is equivalent to
//! find_acceptable_drop(), but it does not get forwarded to its container.
//! Otherwise this may result in infnite recursion, since if the element
//! does not accept the dragged content, it will recurse back to the same
//! parent that forwarded it back to the element.

template<typename element_impl>
class drag_destination_elementObj : public element_impl {

 public:
	typedef element_impl superclass_t;

	using superclass_t::superclass_t;

	bool find_acceptable_drop(ONLY IN_THREAD,
				  ref<elementObj::implObj> &accepting_element,
				  const source_dnd_formats_t &source_formats,
				  xcb_timestamp_t timestamp) override;

	bool this_element_accepts_this_drop
		(ONLY IN_THREAD,
		 ref<elementObj::implObj> &accepting_element,
		 const source_dnd_formats_t &source_formats,
		 xcb_timestamp_t timestamp);

	//! Implemented by the display element.
	virtual bool accepts_drop(ONLY IN_THREAD,
				  const source_dnd_formats_t &formats,
				  xcb_timestamp_t timestamp)
		=0;
};

LIBCXXW_NAMESPACE_END

#endif
