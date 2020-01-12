/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef element_position_updated_h
#define element_position_updated_h

#include "x/w/namespace.H"
#include "x/w/elementobj.H"
#include "element_position_updatedfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include <map>
#include <unordered_map>
#include <unordered_set>

LIBCXXW_NAMESPACE_START

//! Widgets that have been repositioned.

//! Each repositioned widget gets added to this container. The connection
//! thread then processed all repositioned widgets.

class element_position_updated_t {


public:

	//! Sort repositioned widgets

	//! The repositioned widgets get sorted, first, by their nesting_level.
	//! process_updated_position()/process_same_position() gets invoked
	//! starting with the top-level widgets first (smaller to higher
	//! nesting levels).
	//!
	//! The second level container is a map of each repositioned widget's
	//! parent widget, and all repositioned widgets in that container
	//! get dropped into a set of them. We end up with sets of
	//! repositioned widgets in each container.

	typedef std::map<size_t,
			 std::unordered_map<elementObj::implObj *,
					    std::unordered_set<element_impl>>
			 > set_t;
private:

	//! Container fo the repositioned widgets
	set_t set_thread_only;
public:
	THREAD_DATA_ONLY(set);
};

LIBCXXW_NAMESPACE_END

#endif
