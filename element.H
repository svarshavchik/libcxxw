/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef element_h
#define element_h

#include "x/w/rectangle.H"
#include "x/w/screenfwd.H"
#include "x/w/generic_windowobj.H"
#include "connection_threadfwd.H"
#include "elementfwd.H"
#include "draw_infofwd.H"
#include "picture.H"
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Base class for element implementation.

//! Stuff that all display elements have.

class LIBCXX_HIDDEN elementObj::implObj : virtual public obj {

	//! Most of the stuff is for the connection thread's consumption, only.

	struct data_thread_only_t {

		//! Nesting level.

		//! Top level window elements have a nesting level of 0.
		//! All others have one more than their parent element.
		size_t nesting_level;

		//! Current position
		rectangle current_position;


		////////////////////////////////////////////////////////////
		//
		// Defaulted values.

		//! Requested visibility.

		//! show()/hide() sets this, then adds this element to the
		//! visibility_updated set.

		bool requested_visibility=false;

		//! Actual visibility.

		//! When the connection thread is done with its
		//! work, it goes through all elements in visibility_updated
		//! and if requested_visibility != actual_visibility,
		//! update_visibility gets called.

		bool actual_visibility=false;
	};

	//! For the connection thread's use, only.

	data_thread_only_t data_thread_only;

 public:
	//! Return my window

	virtual generic_windowObj::handlerObj &get_window_handler()=0;

	//! Return my window
	virtual const generic_windowObj::handlerObj &get_window_handler() const=0;
	//! Return the element's screen
	screen get_screen();

	//! Return the element's screen
	const_screen get_screen() const;

	// We assume that anything that uses the comparison function will
	// enforce thread_only.

	friend class elements_by_nesting_level;

	//! Constructor
	implObj(size_t nesting_level,
		const rectangle &initial_position);

	//! Destructor
	~implObj();

	THREAD_DATA_ONLY(data);
 public:

	/////////////////////////////////////////////////////////////////////
	//

	//! show() or hide() this display element.

	void request_visibility(bool flag);

	//! Executed in the connection thread.

	//! If different visibility was updated, invoke update_visibility().

	void update_visibility(IN_THREAD_ONLY);

	//! update_visibility() invokes this callback.

	virtual void visibility_updated(IN_THREAD_ONLY, bool flag);

	//! Return my viewport.

	//! The element calls this if it wants to draw itself.
	//! It passes its current_position for the initial_viewport.
	//! The implementation may modify it, before returning the final
	//! viewport in get_draw_info().

	virtual draw_info get_draw_info(IN_THREAD_ONLY,
					const rectangle &initial_viewport)=0;

	class clip_region_set;

	//! Clear to background color

	//! Wrapper that acquires a clip_region_set, first.

	void clear_to_color(IN_THREAD_ONLY,
			    const draw_info &di,
			    const std::set<rectangle> &areas);

	//! Clear to background color

	//! Wrapper that acquires a clip_region_set, first.
	//! The rectangles' coordinates are relative to the viewport.

	void clear_to_color(IN_THREAD_ONLY,
			    const clip_region_set &,
			    const draw_info &di,
			    const std::set<rectangle> &areas);
};

inline bool elements_by_nesting_level::operator()(const elementimpl &a,
						  const elementimpl &b) const
{
	if (a->data_thread_only.nesting_level < b->data_thread_only.nesting_level)
		return true;

	if (b->data_thread_only.nesting_level < a->data_thread_only.nesting_level)
		return false;

	return a < b;
}

LIBCXXW_NAMESPACE_END

#endif
