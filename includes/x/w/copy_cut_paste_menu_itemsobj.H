/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_copy_cut_paste_menu_itemsobj_h
#define x_w_copy_cut_paste_menu_itemsobj_h

#include <x/w/copy_cut_paste_menu_itemsfwd.H>
#include <x/w/connection_threadfwd.H>
#include <x/obj.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see copy_cut_paste_menu_items

class copy_cut_paste_menu_itemsObj: virtual public obj {

public:
	//! Constructor
	copy_cut_paste_menu_itemsObj() LIBCXX_HIDDEN;

	//! Destructor
	~copy_cut_paste_menu_itemsObj() LIBCXX_HIDDEN;

	//! Update menu items' status.
	virtual void update()=0;

	//! Update menu items' status.
	virtual void update(ONLY IN_THREAD)=0;
};

LIBCXXW_NAMESPACE_END

#endif
