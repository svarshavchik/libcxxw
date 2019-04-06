/*
** Copyright 2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_in_thread_new_cells_infoobj_h
#define x_w_in_thread_new_cells_infoobj_h

#include "listlayoutmanager/in_thread_new_cells_infofwd.H"
#include "listlayoutmanager/listlayoutstyle_impl.H"
#include "includes/x/w/listlayoutmanagerfwd.H"

#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see in_thread_new_cells_info

class in_thread_new_cells_infoObj: virtual public obj {

public:
	new_cells_info info;

	in_thread_new_cells_infoObj(const listlayoutmanager &lm,
				    const std::vector<list_item_param> &items);

	~in_thread_new_cells_infoObj();
};

LIBCXXW_NAMESPACE_END

#endif
