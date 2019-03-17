/*
** Copyright 2018-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef drag_source_h
#define drag_source_h

#include "x/w/types.H"
#include "x/w/rectangle.H"
#include "x/w/elementobj.H"
#include "x/w/generic_windowobj.H"
#include "x/w/connection_threadfwd.H"
#include "x/w/defaultthemefwd.H"

#include <x/logger.H>
#include <xcb/xproto.h>
#include <vector>
#include <unordered_set>
#include <optional>
#include <tuple>

LIBCXXW_NAMESPACE_START

//! Implement the source portion of the xdnd protocol.

class LIBCXX_HIDDEN drag_source {

public:

	LOG_CLASS_SCOPE;

	//! Window where drag operation originates
	const xcb_window_t source_window;

	//! Screen where drag operation originates
	const xcb_screen_t * const source_screen;

	//! Available formats for dragged contents.

	//! We do not implement XdndTypeList as of now, this is
	//! limited to three atoms.

	const std::vector<xcb_atom_t> source_formats;


	//! Absolute position of the dragging element.

	//! The element that's getting dragged from, its absolute
	//! position on the screen.
	const coord_t dragging_element_abs_x;

	//! Absolute position of the dragging element.

	//! The element that's getting dragged from, its absolute
	//! position on the screen.
	const coord_t dragging_element_abs_y;

	//! Original cursor pointer when dragging started.

	//! This is relative to the dragging element's position.
	const coord_t start_x;

	//! Original cursor pointer when dragging started.

	//! This is relative to the dragging element's position.
	const coord_t start_y;

	//! When to start dragging.

	//! Dragging officially starts when the pointer moves at least
	//! this much from start_x.

	const dim_t drag_start_horiz;

	//! When to start dragging.

	//! Dragging officially starts when the pointer moves at least
	//! this much from start_y.

	const dim_t drag_start_vert;

	//! Constructor

	drag_source(ONLY IN_THREAD,
		    elementObj::implObj &dragging_element,
		    const std::vector<xcb_atom_t> &source_formats,
		    coord_t start_x,
		    coord_t start_y,
		    dim_t drag_start_horiz,
		    dim_t drag_start_vert);

private:

	//! Delegated constructor

	drag_source(ONLY IN_THREAD,
		    const rectangle &dragging_element_absolute_pos,
		    generic_windowObj::handlerObj &window_handler,
		    const std::vector<xcb_atom_t> &source_formats,
		    coord_t start_x,
		    coord_t start_y,
		    dim_t drag_start_horiz,
		    dim_t drag_start_vert);

	//! Delegated constructor

	drag_source(ONLY IN_THREAD,
		    xcb_window_t source_window,
		    const screen &window_screen,
		    const std::vector<xcb_atom_t> &source_formats,
		    coord_t dragging_element_abs_x,
		    coord_t dragging_element_abs_y,
		    coord_t start_x,
		    coord_t start_y,
		    dim_t drag_start_horiz,
		    dim_t drag_start_vert);
public:
	//! Report pointer movement

	//! The element that's dragging needs to have its
	//! report_motion_event forward motion_event.x and
	//! motion_event.y here, and we take care of the rest.

	void report_motion_event(ONLY IN_THREAD,
				 coord_t x, coord_t y);

private:

	//! Flag updated by report_motion_event().
	bool dragged_sufficiently_for_showing_drag_pointer=false;

	//! Which window is under the pointer
	xcb_window_t window_under_pointer=XCB_NONE;

	//! Other windows we traversed before reached the window_under_pointer.

	//! These windows do not have XdndAware set, so when checking new
	//! pointer coordinates, and we see one of these windows we don't
	//! need to check if the window is XdndAware, because we already
	//! know that it's not.

	std::unordered_set<xcb_window_t> parent_windows_under_pointer;

	//! Current version of the DND protocol being spoken.

	//! If window_under_pointer advertises XdndAware, this is
	//! the negotiated protocol level.
	std::optional<int> version;

	//! This is the XdndAware proxy window.

	//! If the DND destination does not specify a proxy window,
	//! this is same as window_under_pointer
	xcb_window_t proxy_window;

	//! Most recent absolute pointer X coordinate
	coord_t most_recent_absolute_x;

	//! Most recent absolute pointer Y coordinate
	coord_t most_recent_absolute_y;

	//! Most recent server timestamp
	xcb_timestamp_t most_recent_timestamp;

	//! Whether we received a status update from the drop target.
	bool status_received=false;

	//! Whether the drop target will accept the drop.
	bool will_accept_drop=false;

	//! Most recent position sent to client.
	std::optional<std::tuple<coord_t, coord_t>
		      > most_recent_sent_xy;

	//! Target requested no more updates for this rectangle.
	rectangle target_do_not_update;

	//! Show the dragging pointer over something that's droppable.

	virtual void show_droppable_pointer(ONLY IN_THREAD)=0;

	//! Show the dragging pointer over something that's not droppable.

	virtual void show_notdroppable_pointer(ONLY IN_THREAD)=0;
 public:

	//! Leaving the current window under the pointer.

	//! This is because we are either moving to a different window,
	//! or we are going away ourselves.
	void leave_window(ONLY IN_THREAD);

	//! XdndStatus message received from the drop target.

	void status_update(ONLY IN_THREAD,
			   xcb_window_t w,
			   bool flag,
			   const rectangle &r);

	//! Button 1 was released.

	//! If the target indicates that it accepts the drop, we
	//! send the XdndDrop message and return true. We return
	//! false if the target refuses the drop.

	bool drop(ONLY IN_THREAD);
private:
	//! Check if the drop target needs a position update.

	void send_position_update(ONLY IN_THREAD);
};

LIBCXXW_NAMESPACE_END

#endif
