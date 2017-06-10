/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerfwd_h
#define x_w_listlayoutmanagerfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <string>

LIBCXXW_NAMESPACE_START

class listlayoutmanagerObj;

/*! The list layout manager.

*/

typedef ref<listlayoutmanagerObj> listlayoutmanager;

//! A constant list layout manager.

//! \see listlayoutmanager

typedef const_ref<listlayoutmanagerObj> const_listlayoutmanager;

class listlayoutstyle;

//! Specification for padding between list items

struct list_padding {

	//! Padding above and below each list item
	std::string v_padding;

	//! Left margin
	std::string left_padding;

	//! Padding between columns of multicolumn lists
	std::string inner_padding;

	//! Right margin padding
	std::string right_padding;
};

//! Default list padding

list_padding default_list_padding();

//! The list highlights items to indicate they're selected

extern const listlayoutstyle &highlighted_list_style;

LIBCXXW_NAMESPACE_END

#endif
