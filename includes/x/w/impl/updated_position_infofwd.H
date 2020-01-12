/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_impl_updated_position_infofwd_h
#define x_w_impl_updated_position_infofwd_h

#include <x/w/namespace.H>
#include <x/w/elementobj.H>
#include <x/w/rectanglefwd.H>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_set>
#include <x/refptr_hash.H>

LIBCXXW_NAMESPACE_START

struct LIBCXX_HIDDEN updated_position_info;

struct LIBCXX_HIDDEN updated_position_move_info;

//! Temporary list of widgets and their new position.

//! The intermediate list consists of: the widget, it's new position, and
//! a flag that's initially false, and gets set to true if the widget's
//! contents were moved directly in the window_pixmap, and the widget does
//! not need to be redrawn.

typedef std::vector<std::tuple<element_impl, rectangle, bool>
		    > updated_position_widgets_t;

//! Temporary list of all repositioned widgets, by their nesting level

typedef std::map<size_t, updated_position_widgets_t
		 > all_updated_position_widgets_t;

//! Where to find a widget in a temporary list of repositioned widgets.

typedef std::tuple<all_updated_position_widgets_t::iterator,
		   updated_position_widgets_t::iterator
		   > all_updated_position_widgets_t_iterator;

//! Internal container used in widget position update processing.

typedef std::vector<std::tuple<all_updated_position_widgets_t_iterator,
			       updated_position_move_info>
		    > updated_position_container_t;

LIBCXXW_NAMESPACE_END

#endif
