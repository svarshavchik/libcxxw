/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_listlayoutmanagerbasefwd_h
#define x_w_listlayoutmanagerbasefwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>
#include <functional>

LIBCXXW_NAMESPACE_START

class listlayoutmanagerbaseObj;
class list_lock;

/*! Common methods shared by listlayoutmanager and textlistlayoutmanager.

Functions for adding text items to a list layout manager. That's all that
textlistlayoutmanager does. listlayoutmanager also has function for adding
any non-focusable display element.

*/

typedef ref<listlayoutmanagerbaseObj> listlayoutmanagerbase;

//! A constant \ref listlayoutmanagerbase "listlayoutmanagerbase".

//! \see listlayoutmanagerbase

typedef const_ref<listlayoutmanagerbaseObj> const_listlayoutmanagerbase;

//! A nullable pointer reference to a \ref listlayoutmanagerbase "listlayoutmanagerbase".

//! \see listlayoutmanagerbase

typedef ptr<listlayoutmanagerbaseObj> listlayoutmanagerbaseptr;

//! A nullable pointer reference to a const \ref listlayoutmanagerbase "listlayoutmanagerbase".

//! \see listlayoutmanagerbase

typedef const_ptr<listlayoutmanagerbaseObj> const_listlayoutmanagerbaseptr;


//! List item callback's signature

//! The list item callback receives a lock object, the current index of the
//! callback's list item, and whether the list item is currently selected.
typedef void list_item_status_change_callback_t(list_lock &lock,
						size_t i,
						bool selected);

//! A list item callback factory

//! The factory gets called when a new list item gets created. The factory
//! returns a callback for the new list item.
typedef std::function<list_item_status_change_callback_t
		      > list_item_status_change_callback_factory_t();


LIBCXXW_NAMESPACE_END

#endif
